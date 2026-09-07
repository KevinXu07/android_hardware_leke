LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := leke_time_migrate
LOCAL_VENDOR_MODULE := true
LOCAL_SRC_FILES := leke_time_migrate.c
LOCAL_CFLAGS := -Wall -Werror
LOCAL_SHARED_LIBRARIES := libc liblog
include $(BUILD_EXECUTABLE)
