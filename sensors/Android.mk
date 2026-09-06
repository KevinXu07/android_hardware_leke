LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE := android.hardware.sensors@1.0-service.leke
LOCAL_INIT_RC := android.hardware.sensors@1.0-service.leke.rc
LOCAL_SRC_FILES := service.cpp

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libcutils \
    libdl \
    libbase \
    libutils \
    libshim_l05_location \
    libhidlbase \
    libhidltransport \
    android.hardware.sensors@1.0

include $(BUILD_EXECUTABLE)

include $(call all-makefiles-under,$(LOCAL_PATH))
