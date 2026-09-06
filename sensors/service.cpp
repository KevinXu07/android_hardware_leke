/*
 * L05 sensor service wrapper.
 *
 * The stock Android 7 SSC sub-HAL snapshots its sensor list when it is loaded.
 * On L05, the generic class-hal start happens roughly one second before ADSP
 * firmware reaches the ready state, permanently caching an empty list.  Keep
 * the standard Android 8 passthrough service but defer its construction long
 * enough for the already-triggered ADSP boot to finish.
 */

#define LOG_TAG "android.hardware.sensors@1.0-service.leke"

#include <android/hardware/sensors/1.0/ISensors.h>
#include <hidl/LegacySupport.h>
#include <log/log.h>
#include <unistd.h>

using android::hardware::defaultPassthroughServiceImplementation;
using android::hardware::sensors::V1_0::ISensors;

extern "C" void l05_location_shim_init(void);

int main() {
    constexpr unsigned int kAdspStartupDelaySeconds = 3;

    // Keep the Android N compatibility shim resident.  The private legacy
    // location library also lists it as a DT_NEEDED dependency so that symbols
    // remain visible inside the later Multi-HAL dlopen() group.
    l05_location_shim_init();

    ALOGI("Waiting %u seconds for L05 ADSP sensor services",
          kAdspStartupDelaySeconds);
    sleep(kAdspStartupDelaySeconds);

    // One thread blocks in poll(); another must remain available for methods.
    return defaultPassthroughServiceImplementation<ISensors>(2);
}
