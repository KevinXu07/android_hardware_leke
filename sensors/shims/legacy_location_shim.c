/*
 * Compatibility symbols for the L05's Android N Qualcomm location blobs.
 *
 * The stock blobs predate Android O and reference APIs which are no longer
 * exported by the platform or by the newer source-built location core.  Keep
 * these symbols isolated in a device-specific linker shim instead of changing
 * shared Android or Qualcomm libraries.
 */

#include <fcntl.h>
#include <stddef.h>
#include <unistd.h>

__attribute__((visibility("default")))
void l05_location_shim_init(void)
{
}

__attribute__((visibility("default")))
const char *get_process_name(void)
{
    static char name[256] = "unknown";
    int fd;
    ssize_t count;

    fd = open("/proc/self/cmdline", O_RDONLY | O_CLOEXEC);
    if (fd < 0)
        return name;

    count = read(fd, name, sizeof(name) - 1);
    close(fd);
    if (count <= 0) {
        name[0] = 'u';
        name[1] = 'n';
        name[2] = 'k';
        name[3] = 'n';
        name[4] = 'o';
        name[5] = 'w';
        name[6] = 'n';
        name[7] = '\0';
        return name;
    }

    name[count] = '\0';
    return name;
}

/*
 * Android N's libloc_eng calls this legacy C++ method.  The Android O
 * source-built libloc_core only exports reportNmeaEvent().  The SSC sensor
 * path never consumes GNSS NMEA callbacks, so a no-op compatibility symbol
 * is safer than mixing the complete, incompatible stock libloc_core into the
 * active GNSS stack.
 */
__attribute__((visibility("default")))
void l05_legacy_report_nmea(void *adapter, const char *nmea, int length)
    __asm__("_ZN8loc_core14LocAdapterBase10reportNmeaEPKci");

void l05_legacy_report_nmea(void *adapter, const char *nmea, int length)
{
    (void)adapter;
    (void)nmea;
    (void)length;
}
