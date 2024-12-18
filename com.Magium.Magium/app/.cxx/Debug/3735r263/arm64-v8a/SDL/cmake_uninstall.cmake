if (NOT EXISTS "/home/alaam/cpp/magium/Magium-SDL/com.Magium.Magium/app/.cxx/Debug/3735r263/arm64-v8a/install_manifest.txt")
    message(FATAL_ERROR "Cannot find install manifest: \"/home/alaam/cpp/magium/Magium-SDL/com.Magium.Magium/app/.cxx/Debug/3735r263/arm64-v8a/install_manifest.txt\"")
endif()

file(READ "/home/alaam/cpp/magium/Magium-SDL/com.Magium.Magium/app/.cxx/Debug/3735r263/arm64-v8a/install_manifest.txt" files)
string(REGEX REPLACE "\n" ";" files "${files}")
foreach(file ${files})
    message(STATUS "Uninstalling \"$ENV{DESTDIR}${file}\"")
    execute_process(
        COMMAND /home/alaam/Android/Sdk/cmake/3.22.1/bin/cmake -E remove "$ENV{DESTDIR}${file}"
        OUTPUT_VARIABLE rm_out
        RESULT_VARIABLE rm_retval
    )
    if(NOT ${rm_retval} EQUAL 0)
        message(FATAL_ERROR "Problem when removing \"$ENV{DESTDIR}${file}\"")
    endif (NOT ${rm_retval} EQUAL 0)
endforeach()
