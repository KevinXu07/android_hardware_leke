/*
 * Copyright (C) 2026 KevinXu07
 * SPDX-License-Identifier: Apache-2.0
 *
 * Seed Qualcomm time-services after a data wipe.  L05 factory firmware keeps
 * valid generic offsets in /persist/time, but the Android 8 daemon selects
 * /data/time and does not fall back when that directory exists but is empty.
 */

#define LOG_TAG "L05TimeMigrate"

#include <android/log.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

static bool valid_offset_file(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode) && st.st_size == 8;
}

static int copy_missing_offset(int base) {
    char source[64];
    char target[64];
    char temporary[72];
    unsigned char value[8];

    snprintf(source, sizeof(source), "/persist/time/ats_%d", base);
    snprintf(target, sizeof(target), "/data/time/ats_%d", base);
    snprintf(temporary, sizeof(temporary), "/data/time/.ats_%d.tmp", base);

    if (valid_offset_file(target) || !valid_offset_file(source)) {
        return 0;
    }

    int src = open(source, O_RDONLY | O_CLOEXEC);
    if (src < 0) {
        ALOGE("open %s failed: %s", source, strerror(errno));
        return -1;
    }
    ssize_t count = read(src, value, sizeof(value));
    close(src);
    if (count != (ssize_t)sizeof(value)) {
        ALOGE("read %s returned %zd", source, count);
        return -1;
    }

    int dst = open(temporary, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0600);
    if (dst < 0) {
        ALOGE("open %s failed: %s", temporary, strerror(errno));
        return -1;
    }
    count = write(dst, value, sizeof(value));
    int sync_result = fsync(dst);
    int saved_errno = errno;
    close(dst);
    if (count != (ssize_t)sizeof(value) || sync_result != 0) {
        errno = saved_errno;
        ALOGE("write %s failed: %s", temporary, strerror(errno));
        unlink(temporary);
        return -1;
    }
    if (rename(temporary, target) != 0) {
        ALOGE("rename %s failed: %s", target, strerror(errno));
        unlink(temporary);
        return -1;
    }
    chmod(target, 0600);
    ALOGI("seeded %s from factory persistence", target);
    return 1;
}

int main(void) {
    int copied = 0;
    int failures = 0;

    for (int base = 1; base <= 15; ++base) {
        int result = copy_missing_offset(base);
        if (result > 0) {
            ++copied;
        } else if (result < 0) {
            ++failures;
        }
    }
    ALOGI("finished: copied=%d failures=%d", copied, failures);
    return failures == 0 ? 0 : 1;
}
