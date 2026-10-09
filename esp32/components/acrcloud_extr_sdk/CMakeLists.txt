# AFP fingerprint extractor -- prebuilt static library (source not included).
# The implementation is chosen per chip; nothing to configure. Only ESP32 / ESP32-S3 / ESP32-P4
# use esp-dsp; on the other chips esp-dsp is neither downloaded nor linked.
set(_afp_dsp OFF)
if(IDF_TARGET MATCHES "^(esp32|esp32s3|esp32p4)$")
    set(_afp_dsp ON)
endif()

if(_afp_dsp)
    idf_component_register(INCLUDE_DIRS "include" PRIV_REQUIRES espressif__esp-dsp)
else()
    idf_component_register(INCLUDE_DIRS "include")
endif()

if(NOT CMAKE_BUILD_EARLY_EXPANSION)
    set(_variant ${IDF_TARGET})
    # ESP32 + PSRAM with the cache-bug workaround needs code built with -mfix-esp32-psram-cache-issue
    if(IDF_TARGET STREQUAL "esp32" AND CONFIG_SPIRAM_CACHE_WORKAROUND)
        set(_variant esp32_psram)
    endif()
    set(_lib "${CMAKE_CURRENT_LIST_DIR}/lib/${_variant}/libacrcloud_extr_sdk.a")
    if(NOT EXISTS "${_lib}")
        message(FATAL_ERROR "acrcloud_extr_sdk: no prebuilt library for '${_variant}' (${_lib})")
    endif()
    if(_afp_dsp)
        if(CONFIG_DSP_ANSI)
            message(FATAL_ERROR "acrcloud_extr_sdk: needs esp-dsp built with optimizations (menuconfig: DSP Library -> DSP Optimization -> Optimized)")
        endif()
        add_prebuilt_library(afp_extr_prebuilt "${_lib}" PRIV_REQUIRES espressif__esp-dsp)
    else()
        add_prebuilt_library(afp_extr_prebuilt "${_lib}")
    endif()
    target_link_libraries(${COMPONENT_LIB} INTERFACE afp_extr_prebuilt m)
endif()
