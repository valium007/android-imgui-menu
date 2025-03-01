LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)
LOCAL_MODULE    := libxhook
LOCAL_SRC_FILES := libxhook.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE     := draw
LOCAL_SRC_FILES  := draw.cpp imgui/imgui.cpp \
				imgui/imgui_demo.cpp \
				imgui/imgui_draw.cpp \
				imgui/imgui_impl_android.cpp \
				imgui/imgui_impl_opengl3.cpp \
				imgui/imgui_tables.cpp \
				imgui/imgui_widgets.cpp


LOCAL_STATIC_LIBRARIES := libxhook
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_CFLAGS     := -fvisibility=hidden
LOCAL_CPPFLAGS += -std=c++17
LOCAL_LDLIBS     := -lEGL -lGLESv2 -lGLESv3 -llog -landroid -lz

include $(BUILD_SHARED_LIBRARY)
