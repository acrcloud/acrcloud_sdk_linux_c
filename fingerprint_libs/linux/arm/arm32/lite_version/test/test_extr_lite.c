/*
 * Functional test of libacrcloud_extr_tool_lite.
 *
 *   ./test_extr_lite <in.wav> [out_prefix]
 *
 * Creates query/db audio fingerprints and humming fingerprint from a 16 bit pcm wav
 * (any channels / sample rate).
 * If out_prefix is given, writes <out_prefix>.query.fp / .db.fp / .hum.fp.
 * Exit code is 0 only if all checks pass.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dll_acr_extr_tool_lite.h"

/*
 * Minimal RIFF/WAVE reader: 16 bit PCM only.
 * Returns the pcm data (free by caller) or NULL on error.
 */
static char *read_wav(const char *path, int *pcm_len, int *nchannels, int *sample_rate)
{
    FILE    *f = fopen(path, "rb");
    char     id[4];
    uint32_t size = 0;
    uint16_t fmt_tag = 0, ch = 0, bits = 0;
    uint32_t sr = 0;
    char    *pcm = NULL;

    if (f == NULL) {
        fprintf(stderr, "open %s fail\n", path);
        return NULL;
    }
    if (fread(id, 1, 4, f) != 4 || memcmp(id, "RIFF", 4) != 0 || fread(&size, 4, 1, f) != 1 ||
        fread(id, 1, 4, f) != 4 || memcmp(id, "WAVE", 4) != 0) {
        fprintf(stderr, "%s is not a wav file\n", path);
        goto end;
    }

    while (fread(id, 1, 4, f) == 4 && fread(&size, 4, 1, f) == 1) {
        if (memcmp(id, "fmt ", 4) == 0) {
            long next = ftell(f) + size + (size & 1);
            if (fread(&fmt_tag, 2, 1, f) != 1 || fread(&ch, 2, 1, f) != 1 || fread(&sr, 4, 1, f) != 1 ||
                fseek(f, 6, SEEK_CUR) != 0 || fread(&bits, 2, 1, f) != 1) {
                break;
            }
            fseek(f, next, SEEK_SET);
        } else if (memcmp(id, "data", 4) == 0) {
            if (ch == 0 || bits != 16 || (fmt_tag != 1 && fmt_tag != 0xFFFE)) {
                fprintf(stderr, "%s: only 16 bit pcm wav is supported\n", path);
                break;
            }
            pcm = (char *)malloc(size);
            if (pcm == NULL || fread(pcm, 1, size, f) != size) {
                fprintf(stderr, "%s: read data fail\n", path);
                free(pcm);
                pcm = NULL;
                break;
            }
            *pcm_len = (int)size;
            *nchannels = ch;
            *sample_rate = (int)sr;
            break;
        } else {
            fseek(f, size + (size & 1), SEEK_CUR);
        }
    }

end:
    fclose(f);
    return pcm;
}

static int n_fail = 0;

#define CHECK(cond, ...)                                                                                               \
    do {                                                                                                               \
        if (!(cond)) {                                                                                                 \
            n_fail++;                                                                                                  \
            printf("  FAIL: ");                                                                                        \
        } else {                                                                                                       \
            printf("  ok:   ");                                                                                        \
        }                                                                                                              \
        printf(__VA_ARGS__);                                                                                           \
        printf("\n");                                                                                                  \
    } while (0)

static void save(const char *prefix, const char *suffix, char *buf, int len)
{
    char  path[4096];
    FILE *f;

    if (prefix == NULL || buf == NULL) {
        return;
    }
    snprintf(path, sizeof(path), "%s%s", prefix, suffix);
    f = fopen(path, "wb");
    if (f != NULL) {
        fwrite(buf, 1, len, f);
        fclose(f);
    }
}

int main(int argc, char **argv)
{
    const char *out_prefix = argc > 2 ? argv[2] : NULL;
    char       *pcm, *pcm_copy;
    int         pcm_len = 0, nchannels = 0, sample_rate = 0;
    char       *fp = NULL;
    int         fp_len = 0;

    if (argc < 2) {
        fprintf(stderr, "usage: %s <in.wav> [out_prefix]\n", argv[0]);
        return 2;
    }

    pcm = read_wav(argv[1], &pcm_len, &nchannels, &sample_rate);
    if (pcm == NULL) {
        return 2;
    }
    printf("%s: pcm_len=%d channels=%d sample_rate=%d\n", argv[1], pcm_len, nchannels, sample_rate);

    CHECK(acr_get_version() != NULL && acr_get_version()[0] != '\0', "sdk version: %s", acr_get_version());

    /* the library must not modify the caller's buffer */
    pcm_copy = (char *)malloc(pcm_len);
    memcpy(pcm_copy, pcm, pcm_len);

    acr_create_audio_fingerprint_by_pcm(pcm, pcm_len, 0, nchannels, sample_rate, &fp, &fp_len);
    CHECK(fp != NULL && fp_len > 0, "query fingerprint len=%d", fp_len);
    save(out_prefix, ".query.fp", fp, fp_len);
    acr_free(fp);

    acr_create_audio_fingerprint_by_pcm(pcm, pcm_len, 1, nchannels, sample_rate, &fp, &fp_len);
    CHECK(fp != NULL && fp_len > 0, "db fingerprint len=%d", fp_len);
    save(out_prefix, ".db.fp", fp, fp_len);
    acr_free(fp);

    acr_create_humming_fingerprint_by_pcm(pcm, pcm_len, nchannels, sample_rate, &fp, &fp_len);
    CHECK(fp != NULL && fp_len > 0, "humming fingerprint len=%d", fp_len);
    save(out_prefix, ".hum.fp", fp, fp_len);
    acr_free(fp);

    CHECK(memcmp(pcm, pcm_copy, pcm_len) == 0, "input pcm buffer unchanged");

    /* error cases */
    fp = (char *)1;
    acr_create_audio_fingerprint_by_pcm(pcm, 100, 0, nchannels, sample_rate, &fp, &fp_len);
    CHECK(fp == NULL && fp_len == 0, "too short input returns NULL");
    fp = (char *)1;
    acr_create_audio_fingerprint_by_pcm(NULL, pcm_len, 0, nchannels, sample_rate, &fp, &fp_len);
    CHECK(fp == NULL && fp_len == 0, "NULL input returns NULL");
    fp = (char *)1;
    acr_create_humming_fingerprint_by_pcm(pcm, pcm_len, 0, sample_rate, &fp, &fp_len);
    CHECK(fp == NULL && fp_len == 0, "invalid channels returns NULL");
    acr_free(NULL);

    free(pcm);
    free(pcm_copy);

    printf("%s\n", n_fail == 0 ? "ALL PASSED" : "FAILED");
    return n_fail == 0 ? 0 : 1;
}
