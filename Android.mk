LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE     := draw

LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/imgui/*.cpp) \
			$(wildcard $(LOCAL_PATH)/xhook/*.c) \
			draw.cpp


LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_CFLAGS     := -fvisibility=hidden
LOCAL_CPPFLAGS += -std=c++17
LOCAL_LDLIBS     := -lEGL -lGLESv2 -lGLESv3 -llog -landroid -lz

include $(BUILD_SHARED_LIBRARY)
