# Leke display HAL fork

This repository is the L05-specific fork of the LineageOS 15.1 CAF msm8996
display HAL. It is selected from `device/leke/L05/BoardConfig.mk` with:

```make
USE_DEVICE_SPECIFIC_DISPLAY := true
DEVICE_SPECIFIC_DISPLAY_PATH := hardware/leke/display
```

The fork intentionally keeps the original module names (`gralloc.msm8953`,
`hwcomposer.msm8953`, and related display modules). The device-specific path
map makes the stock `hardware/qcom/display-caf/msm8996` Android.mk guard skip
the shared project, so both copies are not built together.

L05-specific changes currently carried here:

* provide the legacy `gralloc_module_t::perform` callback used by the stock
  Android 7 Adreno EGL stack;
* disable the crashing legacy SDM color-manager path by default, while keeping
  the `sdm.disable_color_manager=0` opt-in for diagnostics.

The `hardware/qcom/display-caf/msm8996` source remains unmodified in the main
tree. Use the local manifest snippet in `analysis/leke-display-local-manifest.xml`
after this repository has been pushed to GitHub.
