LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE    := Perseus

LOCAL_CPPFLAGS := -w -std=c++17
LOCAL_LDLIBS := -llog
LOCAL_C_INCLUDES += $(LOCAL_PATH)

LOCAL_SRC_FILES := Main.cpp \
	Substrate/hde64.c \
	Substrate/SubstrateDebug.cpp \
	Substrate/SubstrateHook.cpp \
	Substrate/SubstratePosixMemory.cpp \
	Substrate/SymbolFinder.cpp \
	And64InlineHook/And64InlineHook.cpp \

include $(BUILD_SHARED_LIBRARY)
