#ifndef _AFP_EXTR_H
#define _AFP_EXTR_H

/*
 * Audio fingerprint extractor for ESP32.
 *
 * Input:  8000 Hz, mono, signed 16-bit PCM, fed in blocks of any size.
 * Output: an opaque fingerprint, sent as-is to the ACRCloud identify API
 *         (data_type = "fingerprint").
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* version of this header; compare with afp_extr_version() of the linked library */
#define AFP_EXTR_VERSION "1.0.5"

#define AFP_SAMPLE_RATE 8000 /* required input sample rate */

/* opaque session handle */
typedef struct afp_extr_session_s afp_extr_session_t;

/*
 * User configuration, 64 bytes. Always start from AFP_EXTR_CONFIG_DEFAULT, then
 * change fields. Future versions add fields only by taking them from reserved_,
 * where 0 means "default", so code built against this header keeps working.
 */
#define AFP_EXTR_CONFIG_VERSION 1

typedef struct {
    uint32_t version_;                  /* AFP_EXTR_CONFIG_VERSION */
    uint8_t  is_local_;                 /* fingerprint mode: 1 (default) or 0 (smaller fingerprint) */
    uint8_t  pad0_[3];                  /* must be 0 */
    int32_t  filter_energy_min_;        /* minimum level of the kept features (default 10) */
    int32_t  silence_energy_threshold_; /* reserved, keep the default (10) */
    float    silence_rate_threshold_;   /* reserved, keep the default (1.0) */
    uint32_t reserved_[11];             /* must be 0 */
} afp_extr_config_t;

#define AFP_EXTR_CONFIG_DEFAULT {AFP_EXTR_CONFIG_VERSION, 1, {0, 0, 0}, 10, 10, 1.0f, {0}}

/*
 * config: NULL for the defaults. Returns NULL if out of memory, or if the config
 * is invalid: version_ is 0 or newer than this library, or pad0_ / reserved_ are not 0.
 */
afp_extr_session_t *afp_extr_create(const afp_extr_config_t *config);

/* feed any number of samples; returns 0, or -1 on error / out of memory */
int afp_extr_feed(afp_extr_session_t *s, const int16_t *pcm, size_t nsamples);

/*
 * feed the audio of a wav file (8000 Hz, mono, 16-bit PCM; extra chunks such as
 * LIST are skipped). Works with SPIFFS / FATFS / SD card VFS. Returns 0, or -1 if
 * the file cannot be read or has another format.
 */
int afp_extr_feed_wav_file(afp_extr_session_t *s, const char *path);

/* end of audio; returns 0, or -1 (e.g. less than 1 second of audio) */
int afp_extr_finish(afp_extr_session_t *s);

/*
 * the fingerprint, after a successful afp_extr_finish(). *fp stays valid until
 * afp_extr_reset() or afp_extr_destroy(). Returns 0, or -1 if there is no fingerprint.
 */
int afp_extr_get_fingerprint(afp_extr_session_t *s, const void **fp, size_t *len);

/*
 * start a new fingerprint with the same session (e.g. one every 10 s): clears the
 * audio and the result, keeps the configuration and all memory, so nothing is
 * allocated again. Copy the previous fingerprint first if it is still needed.
 * Returns 0, or -1 if s is NULL.
 */
int afp_extr_reset(afp_extr_session_t *s);

/* number of samples fed so far */
int64_t afp_extr_samples(const afp_extr_session_t *s);

void afp_extr_destroy(afp_extr_session_t *s);

/* version string of the library, e.g. "1.0.5" */
const char *afp_extr_version(void);

/*
 * Errors are reported only through return values; the library prints nothing.
 * Sessions are independent; one session must not be used from two tasks at the same time.
 */

#ifdef __cplusplus
}
#endif

#endif
