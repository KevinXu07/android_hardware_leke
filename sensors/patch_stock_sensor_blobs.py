#!/usr/bin/env python3
"""Build the isolated Android N location dependency chain for L05 sensors.

The stock 64-bit SSC sub-HAL requires Android N libloc_* ABIs, while the
source-built GNSS service requires Android O libraries with the same SONAMEs.
Rename only the SSC dependency chain instead of replacing the active GNSS
libraries globally.  Inputs and outputs are hash guarded for reproducibility.
"""

import hashlib
import shutil
import subprocess
from pathlib import Path


HARDWARE_ROOT = Path(__file__).resolve().parent
SOURCE_ROOT = Path(__file__).resolve().parents[3]
DEVICE_ROOT = SOURCE_ROOT / "device/leke/L05"
VENDOR_LIB64 = SOURCE_ROOT / "vendor/leke/L05/proprietary/vendor/lib64"
LOCATION64 = DEVICE_ROOT / "prebuilt/stock/location64"
OUTPUT = HARDWARE_ROOT / "prebuilt"

INPUTS = {
    "sensors.ssc.so": (
        VENDOR_LIB64 / "sensors.ssc.so",
        "61e4ceaedcbfb6dbc3f7ad2e930bd4d661af28fe13fec322233024372edc31f6",
    ),
    "liblocationservice_l05.so": (
        VENDOR_LIB64 / "liblocationservice.so",
        "ee1298781e8b6a30fe74367b5cf6a2db5bb1144028641167cae5f3e130f491ca",
    ),
    "libgps_utils_l05.so": (
        LOCATION64 / "libgps.utils.so",
        "065d6ae71c1ea9f81fdbc3f0612bc6cf3cb92a9630b8ff09aae9c09a8018563f",
    ),
    "libloc_core_l05.so": (
        LOCATION64 / "libloc_core.so",
        "96182a246305cba2c4ce794a4389c56364e04e494aebbcef44c15343c7b00301",
    ),
    "libloc_eng_l05.so": (
        LOCATION64 / "libloc_eng.so",
        "90db3061b033a1fef3c48ed429dd51d6295d3fd994489525452775b316afd79d",
    ),
    "libloc_pla_l05.so": (
        LOCATION64 / "libloc_pla.so",
        "4c54e2753d83ae11c0d9a404e3732ee85050069a33cf4c03387d459aa1432a1b",
    ),
}

OUTPUT_SHA256 = {
    "sensors.ssc.so": "44211abf949c5bc434201c601af54e54af2fed2b4244e98ac8acc07300eec119",
    "liblocationservice_l05.so": "569905d50c513a2d9c2fcac2868e8c885ad32ee7be272953f88a87ffd6aa1a9c",
    "libgps_utils_l05.so": "28e418c0f50d39cafcb135f81e2eb04b5f3a84336665afe1d172f6179b5a9c3a",
    "libloc_core_l05.so": "24863160ef28a9e271d8f2994caad5e7d86c8329fc2f6a8b41179f458840fd73",
    "libloc_eng_l05.so": "27c4a8aef3c46b041e279508c466e8b3ad325036b05b65035b0522b9785f7f26",
    "libloc_pla_l05.so": "3ab7a0793cb348778c4817d50f8781ca55599b213a64f8f443b597652d8a9591",
}


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def patchelf(*args):
    subprocess.run(("patchelf",) + args, check=True)


def replace(path, old, new):
    patchelf("--replace-needed", old, new, str(path))


def soname(path, name):
    patchelf("--set-soname", name, str(path))


def main():
    if shutil.which("patchelf") is None:
        raise SystemExit("patchelf is required (Ubuntu package: patchelf)")

    OUTPUT.mkdir(parents=True, exist_ok=True)
    for output_name, (source, expected) in INPUTS.items():
        actual = digest(source)
        if actual != expected:
            raise SystemExit(
                "refusing unknown input {}: sha256={}, expected={}".format(
                    source, actual, expected
                )
            )
        shutil.copyfile(str(source), str(OUTPUT / output_name))

    sensors = OUTPUT / "sensors.ssc.so"
    location = OUTPUT / "liblocationservice_l05.so"
    gps_utils = OUTPUT / "libgps_utils_l05.so"
    loc_core = OUTPUT / "libloc_core_l05.so"
    loc_eng = OUTPUT / "libloc_eng_l05.so"
    loc_pla = OUTPUT / "libloc_pla_l05.so"

    replace(sensors, "liblocationservice.so", "liblocationservice_l05.so")

    soname(location, "liblocationservice_l05.so")
    replace(location, "libloc_core.so", "libloc_core_l05.so")
    replace(location, "libloc_eng.so", "libloc_eng_l05.so")
    replace(location, "libgps.utils.so", "libgps_utils_l05.so")
    replace(location, "libloc_pla.so", "libloc_pla_l05.so")
    # libizat_core.so is loaded as part of this private Android N dependency
    # group and still imports get_process_name().  Make the compatibility shim
    # a real member of the same group; merely linking it into the HIDL service
    # leaves it outside the later dlopen() lookup scope.
    patchelf("--add-needed", "libshim_l05_location.so", str(location))

    soname(loc_core, "libloc_core_l05.so")
    replace(loc_core, "libgps.utils.so", "libgps_utils_l05.so")
    replace(loc_core, "libloc_pla.so", "libloc_pla_l05.so")

    soname(loc_eng, "libloc_eng_l05.so")
    replace(loc_eng, "libloc_core.so", "libloc_core_l05.so")
    replace(loc_eng, "libgps.utils.so", "libgps_utils_l05.so")
    replace(loc_eng, "libloc_pla.so", "libloc_pla_l05.so")

    soname(gps_utils, "libgps_utils_l05.so")
    replace(gps_utils, "libloc_pla.so", "libloc_pla_l05.so")
    soname(loc_pla, "libloc_pla_l05.so")

    for name, expected in OUTPUT_SHA256.items():
        actual = digest(OUTPUT / name)
        if actual != expected:
            raise SystemExit(
                "patched output hash mismatch for {}: {}, expected={}".format(
                    name, actual, expected
                )
            )
        print("{}  {}".format(actual, OUTPUT / name))


if __name__ == "__main__":
    main()
