# ACRCloud Extractor Tool Lite SDK

Version: 1.0.0

Creates ACRCloud audio fingerprints and humming fingerprints from raw PCM audio.
The lite SDK has no decoder: the input must already be 16 bit PCM. Decode your
audio files (mp3, aac, ...) to PCM before calling this SDK.

## Directory layout

```
include/dll_acr_extr_tool_lite.h      API header
lib/libacrcloud_extr_tool_lite.so     shared library
lib/libacrcloud_extr_tool_lite.a      static library
test/test_extr_lite.c                 example / test program
README.md
```

Only the API functions below are exported from the libraries. All internal
symbols are hidden, so they will not conflict with symbols of your program.

## API

```c
void acr_create_audio_fingerprint_by_pcm(char* pcm_buffer, int pcm_buffer_len, char is_db,
                                         int nchannels, int sample_rate,
                                         char** fp_buffer, int* fp_buffer_len);

void acr_create_humming_fingerprint_by_pcm(char* pcm_buffer, int pcm_buffer_len,
                                           int nchannels, int sample_rate,
                                           char** fp_buffer, int* fp_buffer_len);

void acr_free(void* buffer);

const char* acr_get_version(void);
```

| Parameter | Description |
|---|---|
| `pcm_buffer` | PCM data: signed 16 bit, little-endian, interleaved if multi-channel. No WAV header. |
| `pcm_buffer_len` | Length of `pcm_buffer` in **bytes**. |
| `is_db` | `0`: query fingerprint (for recognition). `1`: database fingerprint (for building your own bucket). |
| `nchannels` | Number of channels of `pcm_buffer`, e.g. 1 or 2. |
| `sample_rate` | Sample rate of `pcm_buffer`, e.g. 8000, 16000, 44100, 48000. |
| `fp_buffer` | Output fingerprint. Must be released with `acr_free`. |
| `fp_buffer_len` | Output fingerprint length in bytes. |

Notes:

- Any channel count and sample rate is accepted. The audio is mixed to mono and
  resampled to 8000 Hz internally. 8000 Hz mono input is used as is, without copy
  or resampling.
- `pcm_buffer` is never modified.
- On failure (invalid parameters, audio too short, no fingerprint could be
  extracted), `*fp_buffer` is set to `NULL` and `*fp_buffer_len` to `0`. The audio
  must be at least about 1 second long.
- `acr_free(NULL)` is allowed.
- `acr_get_version` returns a static string, do not free it.
- All functions are reentrant and can be called from multiple threads at the same time.

## Example

```c
#include "dll_acr_extr_tool_lite.h"

char *fp = NULL;
int   fp_len = 0;

/* pcm: 16 bit little-endian, 2 channels, 44100 Hz */
acr_create_audio_fingerprint_by_pcm(pcm, pcm_len, 0, 2, 44100, &fp, &fp_len);
if (fp != NULL) {
    /* send fp / fp_len to ACRCloud identify API */
    acr_free(fp);
}
```

## Build

Static library:

```
gcc -Iinclude your_app.c lib/libacrcloud_extr_tool_lite.a -lm -o your_app
```

Shared library:

```
gcc -Iinclude your_app.c -Llib -l:libacrcloud_extr_tool_lite.so -Wl,-rpath,'$ORIGIN' -o your_app
```

The shared library must then be found at runtime: put it next to `your_app`
(with the rpath above), or set `LD_LIBRARY_PATH`.

## Test program

`test/test_extr_lite.c` reads a 16 bit PCM WAV file (any channels / sample rate),
creates query, database and humming fingerprints, and checks the results.

```
gcc -Iinclude test/test_extr_lite.c lib/libacrcloud_extr_tool_lite.a -lm -o test_extr_lite
./test_extr_lite your.wav [out_prefix]
```

If `out_prefix` is given, the fingerprints are written to
`<out_prefix>.query.fp`, `<out_prefix>.db.fp` and `<out_prefix>.hum.fp`.
The program prints `ALL PASSED` and exits with 0 if all checks pass.
