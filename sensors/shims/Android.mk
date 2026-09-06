LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libshim_l05_location
LOCAL_SRC_FILES := legacy_location_shim.c
LOCAL_CFLAGS := -Wall -Werror
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MULTILIB := both
include $(BUILD_SHARED_LIBRARY)
