/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2024 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "SDL_internal.h"

#if defined(SDL_PLATFORM_WIN32) || defined(SDL_PLATFORM_WINRT) || defined(SDL_PLATFORM_GDK)
#include "core/windows/SDL_windows.h"
#endif

// Simple log messages in SDL

#include "SDL_log_c.h"

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef SDL_PLATFORM_ANDROID
#include <android/log.h>
#endif

#include "stdlib/SDL_vacopy.h"

// The size of the stack buffer to use for rendering log messages.
#define SDL_MAX_LOG_MESSAGE_STACK 256

#define DEFAULT_CATEGORY -1

typedef struct SDL_LogLevel
{
    int category;
    SDL_LogPriority priority;
    struct SDL_LogLevel *next;
} SDL_LogLevel;


// The default log output function
static void SDLCALL SDL_LogOutput(void *userdata, int category, SDL_LogPriority priority, const char *message);

static void SDL_ResetLogPrefixes(void);

static SDL_LogLevel *SDL_loglevels;
static bool SDL_forced_priority = false;
static SDL_LogPriority SDL_forced_priority_level;
static SDL_LogOutputFunction SDL_log_function = SDL_LogOutput;
static void *SDL_log_userdata = NULL;
static SDL_Mutex *log_function_mutex = NULL;

#ifdef HAVE_GCC_DIAGNOSTIC_PRAGMA
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

// If this list changes, update the documentation for SDL_HINT_LOGGING
static const char * const SDL_priority_names[] = {
    NULL,
    "VERBOSE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "CRITICAL"
};
SDL_COMPILE_TIME_ASSERT(priority_names, SDL_arraysize(SDL_priority_names) == SDL_NUM_LOG_PRIORITIES);

static const char *SDL_priority_prefixes[SDL_NUM_LOG_PRIORITIES];

// If this list changes, update the documentation for SDL_HINT_LOGGING
static const char * const SDL_category_names[] = {
    "APP",
    "ERROR",
    "ASSERT",
    "SYSTEM",
    "AUDIO",
    "VIDEO",
    "RENDER",
    "GPU",
    "INPUT",
    "TEST"
};
SDL_COMPILE_TIME_ASSERT(category_names, SDL_arraysize(SDL_category_names) == SDL_LOG_CATEGORY_RESERVED1);

#ifdef HAVE_GCC_DIAGNOSTIC_PRAGMA
#pragma GCC diagnostic pop
#endif

#ifdef SDL_PLATFORM_ANDROID
static int SDL_android_priority[SDL_NUM_LOG_PRIORITIES] = {
    ANDROID_LOG_UNKNOWN,
    ANDROID_LOG_VERBOSE,
    ANDROID_LOG_DEBUG,
    ANDROID_LOG_INFO,
    ANDROID_LOG_WARN,
    ANDROID_LOG_ERROR,
    ANDROID_LOG_FATAL
};
#endif // SDL_PLATFORM_ANDROID

void SDL_InitLog(void)
{
    if (!log_function_mutex) {
        // if this fails we'll try to continue without it.
        log_function_mutex = SDL_CreateMutex();
    }
}

void SDL_QuitLog(void)
{
    SDL_ResetLogPriorities();
    SDL_ResetLogPrefixes();

    if (log_function_mutex) {
        SDL_DestroyMutex(log_function_mutex);
        log_function_mutex = NULL;
    }
}

void SDL_SetLogPriorities(SDL_LogPriority priority)
{
    SDL_LogLevel *entry;

    for (entry = SDL_loglevels; entry; entry = entry->next) {
        entry->priority = priority;
    }

    SDL_forced_priority = true;
    SDL_forced_priority_level = priority;
}

void SDL_SetLogPriority(int category, SDL_LogPriority priority)
{
    SDL_LogLevel *entry;

    for (entry = SDL_loglevels; entry; entry = entry->next) {
        if (entry->category == category) {
            entry->priority = priority;
            return;
        }
    }

    // Create a new entry
    entry = (SDL_LogLevel *)SDL_malloc(sizeof(*entry));
    if (entry) {
        entry->category = category;
        entry->priority = priority;
        entry->next = SDL_loglevels;
        SDL_loglevels = entry;
    }
}

static bool SDL_ParseLogCategory(const char *string, size_t length, int *category)
{
    int i;

    if (SDL_isdigit(*string)) {
        *category = SDL_atoi(string);
        return true;
    }

    if (*string == '*') {
        *category = DEFAULT_CATEGORY;
        return true;
    }

    for (i = 0; i < SDL_arraysize(SDL_category_names); ++i) {
        if (SDL_strncasecmp(string, SDL_category_names[i], length) == 0) {
            *category = i;
            return true;
        }
    }
    return false;
}

static bool SDL_ParseLogPriority(const char *string, size_t length, SDL_LogPriority *priority)
{
    int i;

    if (SDL_isdigit(*string)) {
        i = SDL_atoi(string);
        if (i == 0) {
            // 0 has a special meaning of "disable this category"
            *priority = SDL_NUM_LOG_PRIORITIES;
            return true;
        }
        if (i >= SDL_LOG_PRIORITY_VERBOSE && i < SDL_NUM_LOG_PRIORITIES) {
            *priority = (SDL_LogPriority)i;
            return true;
        }
        return false;
    }

    if (SDL_strncasecmp(string, "quiet", length) == 0) {
        *priority = SDL_NUM_LOG_PRIORITIES;
        return true;
    }

    for (i = SDL_LOG_PRIORITY_VERBOSE; i < SDL_NUM_LOG_PRIORITIES; ++i) {
        if (SDL_strncasecmp(string, SDL_priority_names[i], length) == 0) {
            *priority = (SDL_LogPriority)i;
            return true;
        }
    }
    return false;
}

static bool SDL_ParseLogCategoryPriority(const char *hint, int category, SDL_LogPriority *priority)
{
    const char *name, *next;
    int current_category;

    if (category == DEFAULT_CATEGORY && SDL_strchr(hint, '=') == NULL) {
        return SDL_ParseLogPriority(hint, SDL_strlen(hint), priority);
    }

    for (name = hint; name; name = next) {
        const char *sep = SDL_strchr(name, '=');
        if (!sep) {
            break;
        }
        next = SDL_strchr(sep, ',');
        if (next) {
            ++next;
        }

        if (SDL_ParseLogCategory(name, (sep - name), &current_category)) {
            if (current_category == category) {
                const char *value = sep + 1;
                size_t len;
                if (next) {
                    len = (next - value - 1);
                } else {
                    len = SDL_strlen(value);
                }
                return SDL_ParseLogPriority(value, len, priority);
            }
        }
    }
    return false;
}

static SDL_LogPriority SDL_GetDefaultLogPriority(int category)
{
    const char *hint = SDL_GetHint(SDL_HINT_LOGGING);
    if (hint) {
        SDL_LogPriority priority;

        if (SDL_ParseLogCategoryPriority(hint, category, &priority)) {
            return priority;
        }
        if (SDL_ParseLogCategoryPriority(hint, DEFAULT_CATEGORY, &priority)) {
            return priority;
        }
    }

    switch (category) {
    case SDL_LOG_CATEGORY_APPLICATION:
    case SDL_LOG_CATEGORY_GPU:
        return SDL_LOG_PRIORITY_INFO;
    case SDL_LOG_CATEGORY_ASSERT:
        return SDL_LOG_PRIORITY_WARN;
    case SDL_LOG_CATEGORY_TEST:
        return SDL_LOG_PRIORITY_VERBOSE;
    default:
        return SDL_LOG_PRIORITY_ERROR;
    }
}

SDL_LogPriority SDL_GetLogPriority(int category)
{
    SDL_LogLevel *entry;

    for (entry = SDL_loglevels; entry; entry = entry->next) {
        if (entry->category == category) {
            return entry->priority;
        }
    }

    if (SDL_forced_priority) {
        return SDL_forced_priority_level;
    }

    return SDL_GetDefaultLogPriority(category);
}

void SDL_ResetLogPriorities(void)
{
    SDL_LogLevel *entry;

    while (SDL_loglevels) {
        entry = SDL_loglevels;
        SDL_loglevels = entry->next;
        SDL_free(entry);
    }
    SDL_forced_priority = false;
}

static void SDL_ResetLogPrefixes(void)
{
    for (int i = 0; i < SDL_arraysize(SDL_priority_prefixes); ++i) {
        SDL_priority_prefixes[i] = NULL;
    }
}

static const char *SDL_GetLogPriorityPrefix(SDL_LogPriority priority)
{
    if (priority < SDL_LOG_PRIORITY_VERBOSE || priority >= SDL_NUM_LOG_PRIORITIES) {
        return "";
    }

    if (SDL_priority_prefixes[priority]) {
        return SDL_priority_prefixes[priority];
    }

    switch (priority) {
    case SDL_LOG_PRIORITY_WARN:
        return "WARNING: ";
    case SDL_LOG_PRIORITY_ERROR:
        return "ERROR: ";
    case SDL_LOG_PRIORITY_CRITICAL:
        return "ERROR: ";
    default:
        return "";
    }
}

SDL_bool SDL_SetLogPriorityPrefix(SDL_LogPriority priority, const char *prefix)
{
    if (priority < SDL_LOG_PRIORITY_VERBOSE || priority >= SDL_NUM_LOG_PRIORITIES) {
        return SDL_InvalidParamError("priority");
    }

    if (!prefix) {
        prefix = "";
    } else {
        prefix = SDL_GetPersistentString(prefix);
        if (!prefix) {
            return false;
        }
    }
    SDL_priority_prefixes[priority] = prefix;
    return true;
}

void SDL_Log(SDL_PRINTF_FORMAT_STRING const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, fmt, ap);
    va_end(ap);
}

void SDL_LogVerbose(int category, SDL_PRINTF_FORMAT_STRING const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    SDL_LogMessageV(category, SDL_LOG_PRIORITY_VERBOSE, fmt, ap);
    va_end(ap);
}

void SDL_LogDebug(int category, SDL_PRINTF_FORMAT_STRING const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    SDL_LogMessageV(category, SDL_LOG_PRIORITY_DEBUG, fmt, ap);
    va_end(ap);
}

void SDL_LogInfo(int category, SDL_PRINTF_FORMAT_STRING const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    SDL_LogMessageV(category, SDL_LOG_PRIORITY_INFO, fmt, ap);
    va_end(ap);
}

void SDL_LogWarn(int category, SDL_PRINTF_FORMAT_STRING const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    SDL_LogMessageV(category, SDL_LOG_PRIORITY_WARN, fmt, ap);
    va_end(ap);
}

void SDL_LogError(int category, SDL_PRINTF_FORMAT_STRING const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    SDL_LogMessageV(category, SDL_LOG_PRIORITY_ERROR, fmt, ap);
    va_end(ap);
}

void SDL_LogCritical(int category, SDL_PRINTF_FORMAT_STRING const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    SDL_LogMessageV(category, SDL_LOG_PRIORITY_CRITICAL, fmt, ap);
    va_end(ap);
}

void SDL_LogMessage(int category, SDL_LogPriority priority, SDL_PRINTF_FORMAT_STRING const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    SDL_LogMessageV(category, priority, fmt, ap);
    va_end(ap);
}

#ifdef SDL_PLATFORM_ANDROID
static const char *GetCategoryPrefix(int category)
{
    if (category < SDL_LOG_CATEGORY_RESERVED1) {
        return SDL_category_names[category];
    }
    if (category < SDL_LOG_CATEGORY_CUSTOM) {
        return "RESERVED";
    }
    return "CUSTOM";
}
#endif // SDL_PLATFORM_ANDROID

void SDL_LogMessageV(int category, SDL_LogPriority priority, SDL_PRINTF_FORMAT_STRING const char *fmt, va_list ap)
{
    char *message = NULL;
    char stack_buf[SDL_MAX_LOG_MESSAGE_STACK];
    size_t len_plus_term;
    int len;
    va_list aq;

    // Nothing to do if we don't have an output function
    if (!SDL_log_function) {
        return;
    }

    // Make sure we don't exceed array bounds
    if ((int)priority < 0 || priority >= SDL_NUM_LOG_PRIORITIES) {
        return;
    }

    // See if we want to do anything with this message
    if (priority < SDL_GetLogPriority(category)) {
        return;
    }

    if (!log_function_mutex) {
        // this mutex creation can race if you log from two threads at startup. You should have called SDL_Init first!
        log_function_mutex = SDL_CreateMutex();
    }

    // Render into stack buffer
    va_copy(aq, ap);
    len = SDL_vsnprintf(stack_buf, sizeof(stack_buf), fmt, aq);
    va_end(aq);

    if (len < 0) {
        return;
    }

    // If message truncated, allocate and re-render
    if (len >= sizeof(stack_buf) && SDL_size_add_overflow(len, 1, &len_plus_term) == 0) {
        // Allocate exactly what we need, including the zero-terminator
        message = (char *)SDL_malloc(len_plus_term);
        if (!message) {
            return;
        }
        va_copy(aq, ap);
        len = SDL_vsnprintf(message, len_plus_term, fmt, aq);
        va_end(aq);
    } else {
        message = stack_buf;
    }

    // Chop off final endline.
    if ((len > 0) && (message[len - 1] == '\n')) {
        message[--len] = '\0';
        if ((len > 0) && (message[len - 1] == '\r')) { // catch "\r\n", too.
            message[--len] = '\0';
        }
    }

    SDL_LockMutex(log_function_mutex);
    SDL_log_function(SDL_log_userdata, category, priority, message);
    SDL_UnlockMutex(log_function_mutex);

    // Free only if dynamically allocated
    if (message != stack_buf) {
        SDL_free(message);
    }
}

#if defined(SDL_PLATFORM_WIN32) && !defined(SDL_PLATFORM_WINRT) && !defined(SDL_PLATFORM_GDK)
enum {
    CONSOLE_UNATTACHED = 0,
    CONSOLE_ATTACHED_CONSOLE = 1,
    CONSOLE_ATTACHED_FILE = 2,
    CONSOLE_ATTACHED_MSVC = 3,
    CONSOLE_ATTACHED_ERROR = -1,
} consoleAttached = CONSOLE_UNATTACHED;

// Handle to stderr output of console.
static HANDLE stderrHandle = NULL;
#endif

static void SDLCALL SDL_LogOutput(void *userdata, int category, SDL_LogPriority priority,
                                  const char *message)
{
#if defined(SDL_PLATFORM_WIN32) || defined(SDL_PLATFORM_WINRT) || defined(SDL_PLATFORM_GDK)
    // Way too many allocations here, urgh
    // Note: One can't call SDL_SetError here, since that function itself logs.
    {
        char *output;
        size_t length;
        LPTSTR tstr;
        bool isstack;

#if !defined(SDL_PLATFORM_WINRT) && !defined(SDL_PLATFORM_GDK)
        BOOL attachResult;
        DWORD attachError;
        DWORD consoleMode;
#if !defined(HAVE_STDIO_H)
        DWORD charsWritten;
#endif

        // Maybe attach console and get stderr handle
        if (consoleAttached == CONSOLE_UNATTACHED) {
            attachResult = AttachConsole(ATTACH_PARENT_PROCESS);
            if (!attachResult) {
                attachError = GetLastError();
                if (attachError == ERROR_INVALID_HANDLE) {
                    // This is expected when running from Visual Studio
                    // OutputDebugString(TEXT("Parent process has no console\r\n"));
                    consoleAttached = CONSOLE_ATTACHED_MSVC;
                } else if (attachError == ERROR_GEN_FAILURE) {
                    OutputDebugString(TEXT("Could not attach to console of parent process\r\n"));
                    consoleAttached = CONSOLE_ATTACHED_ERROR;
                } else if (attachError == ERROR_ACCESS_DENIED) {
                    // Already attached
                    consoleAttached = CONSOLE_ATTACHED_CONSOLE;
                } else {
                    OutputDebugString(TEXT("Error attaching console\r\n"));
                    consoleAttached = CONSOLE_ATTACHED_ERROR;
                }
            } else {
                // Newly attached
                consoleAttached = CONSOLE_ATTACHED_CONSOLE;
            }

            if (consoleAttached == CONSOLE_ATTACHED_CONSOLE) {
                stderrHandle = GetStdHandle(STD_ERROR_HANDLE);

                if (GetConsoleMode(stderrHandle, &consoleMode) == 0) {
                    // WriteConsole fails if the output is redirected to a file. Must use WriteFile instead.
                    consoleAttached = CONSOLE_ATTACHED_FILE;
                }
            }
        }
#endif // !defined(HAVE_STDIO_H) && !defined(SDL_PLATFORM_WINRT) && !defined(SDL_PLATFORM_GDK)
        length = SDL_strlen(SDL_GetLogPriorityPrefix(priority)) + SDL_strlen(message) + 1 + 1 + 1;
        output = SDL_small_alloc(char, length, &isstack);
        (void)SDL_snprintf(output, length, "%s%s\r\n", SDL_GetLogPriorityPrefix(priority), message);
        tstr = WIN_UTF8ToString(output);


#if defined(HAVE_STDIO_H) && !defined(SDL_PLATFORM_WINRT) && !defined(SDL_PLATFORM_GDK)
        // When running in MSVC and using stdio, rely on forwarding of stderr to the debug stream
        if (consoleAttached != CONSOLE_ATTACHED_MSVC) {
            // Output to debugger
            OutputDebugString(tstr);
        }
#else
        // Output to debugger
        OutputDebugString(tstr);
#endif

#if !defined(HAVE_STDIO_H) && !defined(SDL_PLATFORM_WINRT) && !defined(SDL_PLATFORM_GDK)
        // Screen output to stderr, if console was attached.
        if (consoleAttached == CONSOLE_ATTACHED_CONSOLE) {
            if (!WriteConsole(stderrHandle, tstr, (DWORD)SDL_tcslen(tstr), &charsWritten, NULL)) {
                OutputDebugString(TEXT("Error calling WriteConsole\r\n"));
                if (GetLastError() == ERROR_NOT_ENOUGH_MEMORY) {
                    OutputDebugString(TEXT("Insufficient heap memory to write message\r\n"));
                }
            }

        } else if (consoleAttached == CONSOLE_ATTACHED_FILE) {
            if (!WriteFile(stderrHandle, output, (DWORD)SDL_strlen(output), &charsWritten, NULL)) {
                OutputDebugString(TEXT("Error calling WriteFile\r\n"));
            }
        }
#endif // !defined(HAVE_STDIO_H) && !defined(SDL_PLATFORM_WINRT) && !defined(SDL_PLATFORM_GDK)

        SDL_free(tstr);
        SDL_small_free(output, isstack);
    }
#elif defined(SDL_PLATFORM_ANDROID)
    {
        char tag[32];

        SDL_snprintf(tag, SDL_arraysize(tag), "SDL/%s", GetCategoryPrefix(category));
        __android_log_write(SDL_android_priority[priority], tag, message);
    }
#elif defined(SDL_PLATFORM_APPLE) && (defined(SDL_VIDEO_DRIVER_COCOA) || defined(SDL_VIDEO_DRIVER_UIKIT))
    /* Technically we don't need Cocoa/UIKit, but that's where this function is defined for now.
     */
    extern void SDL_NSLog(const char *prefix, const char *text);
    {
        SDL_NSLog(SDL_GetLogPriorityPrefix(priority), message);
        return;
    }
#elif defined(SDL_PLATFORM_PSP) || defined(SDL_PLATFORM_PS2)
    {
        FILE *pFile;
        pFile = fopen("SDL_Log.txt", "a");
        if (pFile) {
            (void)fprintf(pFile, "%s%s\n", SDL_GetLogPriorityPrefix(priority), message);
            (void)fclose(pFile);
        }
    }
#elif defined(SDL_PLATFORM_VITA)
    {
        FILE *pFile;
        pFile = fopen("ux0:/data/SDL_Log.txt", "a");
        if (pFile) {
            (void)fprintf(pFile, "%s%s\n", SDL_GetLogPriorityPrefix(priority), message);
            (void)fclose(pFile);
        }
    }
#elif defined(SDL_PLATFORM_3DS)
    {
        FILE *pFile;
        pFile = fopen("sdmc:/3ds/SDL_Log.txt", "a");
        if (pFile) {
            (void)fprintf(pFile, "%s%s\n", SDL_GetLogPriorityPrefix(priority), message);
            (void)fclose(pFile);
        }
    }
#endif
#if defined(HAVE_STDIO_H) && \
    !(defined(SDL_PLATFORM_APPLE) && (defined(SDL_VIDEO_DRIVER_COCOA) || defined(SDL_VIDEO_DRIVER_UIKIT)))
    (void)fprintf(stderr, "%s%s\n", SDL_GetLogPriorityPrefix(priority), message);
#endif
}

void SDL_GetLogOutputFunction(SDL_LogOutputFunction *callback, void **userdata)
{
    if (callback) {
        *callback = SDL_log_function;
    }
    if (userdata) {
        *userdata = SDL_log_userdata;
    }
}

void SDL_SetLogOutputFunction(SDL_LogOutputFunction callback, void *userdata)
{
    SDL_log_function = callback;
    SDL_log_userdata = userdata;
}
