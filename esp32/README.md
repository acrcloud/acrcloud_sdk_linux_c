# AFP Extractor SDK for ESP32

English | [中文](README_zh.md)

Extracts an audio fingerprint from 8 kHz mono PCM on ESP32-series chips. The fingerprint is sent as-is to the ACRCloud identify API for recognition.

- `components/acrcloud_extr_sdk/`: ESP-IDF component with the public header and a **prebuilt static library** for each chip (no source code)
- `examples/demo/`: streaming extraction, timing and memory statistics
- `examples/identify_http/`: extracts a fingerprint and recognizes it through the ACRCloud identify API over HTTPS

## Supported chips

| lib directory | Chip | Notes |
|---|---|---|
| `esp32` | ESP32 | |
| `esp32_psram` | ESP32 + PSRAM | **Selected automatically** when `CONFIG_SPIRAM_CACHE_WORKAROUND` is enabled (built with `-mfix-esp32-psram-cache-issue`) |
| `esp32s3` | ESP32-S3 | |
| `esp32p4` | ESP32-P4 | |
| `esp32s2` `esp32c2` `esp32c3` `esp32c6` `esp32h2` | | Chips without hardware FPU; an implementation optimized for them is used automatically |

The best implementation for each chip is selected automatically; there is nothing to configure. On ESP32 / S3 / P4 the library uses [esp-dsp](https://components.espressif.com/components/espressif/esp-dsp); the ESP-IDF component manager downloads and links it automatically on the first build (nothing to add to your project). Other chips do not use esp-dsp.

Requires ESP-IDF v5.x (verified with v5.3.1).

## Integration

Copy `components/acrcloud_extr_sdk` into your project's `components/` directory, or add this to the project's CMakeLists.txt:

```cmake
set(EXTRA_COMPONENT_DIRS /path/to/afp_extr_sdk/components)
```

Then add `REQUIRES acrcloud_extr_sdk` to your component.

## Usage

```c
#include "afp_extr.h"

afp_extr_config_t   cfg = AFP_EXTR_CONFIG_DEFAULT;   // or pass NULL for the defaults
afp_extr_session_t *s = afp_extr_create(&cfg);
while (recording) {
    afp_extr_feed(s, pcm, n);          // 8 kHz mono int16, any block size (e.g. whatever one I2S read returns)
}
if (afp_extr_finish(s) == 0) {         // returns -1 if less than 1 second of audio was fed
    const void *fp;
    size_t      len;
    afp_extr_get_fingerprint(s, &fp, &len);   // fp stays valid until afp_extr_reset() / afp_extr_destroy()
    /* send fp / len, see examples/identify_http */
}
afp_extr_destroy(s);
```

To recognize repeatedly (e.g. every 10 seconds), create the session once and reuse it with `afp_extr_reset()`. Nothing is allocated again, so the memory stays available even while WiFi/TLS are busy:

```c
afp_extr_session_t *s = afp_extr_create(&cfg);   // once, at start-up
for (;;) {
    /* feed 10 s of audio */
    if (afp_extr_finish(s) == 0 && afp_extr_get_fingerprint(s, &fp, &len) == 0) {
        memcpy(buf, fp, len);                    // fp is invalid after the reset
    }
    afp_extr_reset(s);                           // same configuration, ready for the next 10 s
    /* upload buf while the next segment is being fed */
}
```

- `afp_extr_create()` takes an `afp_extr_config_t` (or NULL for the defaults). **Always** initialize it with `AFP_EXTR_CONFIG_DEFAULT` and then change only what you need:

  | Field | Default | Meaning |
  |---|---|---|
  | `version_` | `AFP_EXTR_CONFIG_VERSION` | Layout version of the structure; set by `AFP_EXTR_CONFIG_DEFAULT`, do not change. |
  | `is_local_` | 1 | Fingerprint mode. 0 gives a smaller fingerprint; the identify API accepts both. |
  | `filter_energy_min_` | 10 | Minimum level of the features kept in the fingerprint. |
  | `silence_energy_threshold_` | 10 | Reserved; keep the default. |
  | `silence_rate_threshold_` | 1.0 | Reserved; keep the default. |
  | `pad0_`, `reserved_` | 0 | Space for future fields; must stay 0. |

  The structure has a fixed size of 64 bytes. Later versions add fields only by taking them from `reserved_` (0 = default), so code written for this version keeps working with newer libraries. `afp_extr_create()` returns NULL if `version_` is 0 or newer than the library, or if `pad0_` / `reserved_` are not 0.

- The fingerprint is an opaque byte string. Send it unchanged as the `sample` of an identify request with `data_type=fingerprint` (see `examples/identify_http`). For 10 seconds of audio it is typically around 2 KB.
- Errors are reported only through return values (NULL or -1). The library never prints anything.
- On single-core chips (C2/C3/C6/H2/S2), do not feed large amounts of audio from a high-priority task without ever yielding the CPU (for example, pushing a whole file or memory buffer in one loop). Otherwise the IDLE task and the watchdogs are starved. Reading from I2S in real time is fine, because the read itself blocks.
- Several sessions can run at the same time in different tasks, but a single session must not be called from two tasks at once.
- Compare `afp_extr_version()` with `AFP_EXTR_VERSION` from the header to make sure the linked library matches the header.
- For audio stored in a file, `afp_extr_feed_wav_file()` accepts 8 kHz / mono / 16-bit wav files. It skips extra chunks such as LIST correctly and works with SPIFFS, FATFS and SD card file systems.

## Resource usage

- **RAM**: per session about 120 KB of heap on ESP32 / S3 / P4 and about 111 KB on the other chips, all in internal RAM (this is what makes it fast). It is split into allocations of at most 12 KB (ESP32 / S3 / P4) or 8 KB (other chips), so it also works on a fragmented heap (e.g. with WiFi/BLE running). The memory is kept until `afp_extr_destroy()`; `afp_extr_reset()` reuses it.
- **Task stack**: the library itself uses less than 2.5 KB.
- **Flash**: about 30–40 KB depending on the chip.
- **CPU**: depends on the chip, its clock and the flash configuration. Run `examples/demo` on your board: it prints the CPU time for 10 s of audio and the real-time factor (share of one core).

## Examples

```bash
. $IDF_PATH/export.sh
cd examples/demo           # or examples/identify_http (configure WiFi and credentials first, see its README)
idf.py set-target esp32
idf.py build flash monitor
```
