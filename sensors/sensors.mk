# L05 stock SSC sensor compatibility payload.
#
# This fragment must be inherited before the generated vendor product.  Android
# 8 keeps the first PRODUCT_COPY_FILES mapping for a destination, which makes
# the vendor tree's unmodified sensors.ssc.so entry harmlessly get ignored.

L05_SENSORS_PATH := hardware/leke/sensors

PRODUCT_COPY_FILES += \
    $(L05_SENSORS_PATH)/prebuilt/sensors.ssc.so:$(TARGET_COPY_OUT_VENDOR)/lib64/sensors.ssc.so \
    $(L05_SENSORS_PATH)/prebuilt/liblocationservice_l05.so:$(TARGET_COPY_OUT_VENDOR)/lib64/liblocationservice_l05.so \
    $(L05_SENSORS_PATH)/prebuilt/libgps_utils_l05.so:$(TARGET_COPY_OUT_VENDOR)/lib64/libgps_utils_l05.so \
    $(L05_SENSORS_PATH)/prebuilt/libloc_core_l05.so:$(TARGET_COPY_OUT_VENDOR)/lib64/libloc_core_l05.so \
    $(L05_SENSORS_PATH)/prebuilt/libloc_eng_l05.so:$(TARGET_COPY_OUT_VENDOR)/lib64/libloc_eng_l05.so \
    $(L05_SENSORS_PATH)/prebuilt/libloc_pla_l05.so:$(TARGET_COPY_OUT_VENDOR)/lib64/libloc_pla_l05.so
