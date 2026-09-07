# L05 stock HAL1 memory callback adapter

The L05 Android 7 HAL passes its QCameraStreamMemory object to
camera_request_memory, whereas Android 8 CameraDevice::sGetMemory expects the
CameraDevice cookie registered through set_callbacks. Stock allocate at VMA
0x85c76 passes r9 (the memory object); allocateMore at 0x85e38 passes r4.
Treating those objects as CameraDevice produces a false missing-callback error
and NULL camera_memory_t allocations.

device/ is the LineageOS 15.1 HAL1 adapter with a bounded callback slot registry.
Each open camera gets a different function pointer, holding a weak owner mapping;
callbacks promote the owner under the registry lock and release the lock before
calling framework Binder. Slots remain reserved until HAL close returns.
This relies on the HAL close contract that callback workers have stopped.

provider/ reuses upstream CameraProvider.cpp, linking our private HAL1 library.
The L05 service links this private provider directly, registers legacy/0, and
keeps the framework Binder driver required by stock QCameraDisplay.
Public hardware/interfaces sources are unchanged.

Device-tree stock-camera patch scripts now restore/verify original blob hashes.
Historical patch tuples are retained to reverse known trial versions, not to
disable preview callbacks or ignore failed cache operations.

Module and full OTA compile passed 2026-09-07. Recovery OTA installed v6.
Both cameras produced visible preview and saved JPEGs; reopening and a normal
reboot also passed capture. Flashlight/video/orientation/long-run tests pending.
