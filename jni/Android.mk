LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libdeclassify
LOCAL_SRC_FILES := lib/libdeclassify.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := NovaX.sh

LOCAL_CFLAGS := -std=c++20 -Wno-error=format-security -w -fno-rtti -fpermissive -fvisibility=hidden -ffunction-sections -fdata-sections -O3 -fomit-frame-pointer
LOCAL_CPPFLAGS := -std=c++20 -Wno-error=format-security -fpermissive -w -Werror -s -fno-rtti -fms-extensions -Wno-error=c++11-narrowing -fvisibility=hidden -O3 -fomit-frame-pointer -ffunction-sections -fdata-sections
LOCAL_CPPFLAGS += -fexceptions

LOCAL_LDFLAGS := -Wl,--gc-sections -Wl,--strip-debug -Wl,-z,relro,-z,now -s -pthread
LOCAL_STRIPFLAGS := --strip-all
LOCAL_STRIP_MODULE := true

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/Draw
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/ImGui
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/ImGui/Touch
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/Fun
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/embree3
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/foundation
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/ImGui/native_surface
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/image

FILE_LIST := $(wildcard $(LOCAL_PATH)/src/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/Draw/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/ImGui/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/ImGui/Touch/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/ImGui/native_surface/*.c*)

LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_STATIC_LIBRARIES := \
    libdeclassify 
    
LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv2 -lGLESv3 -lGLESv1_CM -lz
LOCAL_LDFLAGS += $(LOCAL_PATH)/lib/*.a

include $(BUILD_EXECUTABLE)
