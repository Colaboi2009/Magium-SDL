LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

# Add your application source files here...
LOCAL_SRC_FILES := \
    main.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../SDL/include $(LOCAL_PATH)/../SDL_image/include

LOCAL_SHARED_LIBRARIES := SDL3 SDL3_image

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -lOpenSLES -llog -landroid  # SDL

include $(BUILD_SHARED_LIBRARY)
