/*
 * Fingerprint recognition on ESP32:
 *   x.wav (embedded) -> afp_extr (streamed in blocks) -> fingerprint
 *   -> ACRCloud identify over HTTPS -> print result.
 *
 * In a real product replace the embedded wav with the I2S microphone (8 kHz mono int16).
 */
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "esp_event.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_netif_sntp.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"

#include "acr_identify.h"
#include "afp_extr.h"

static const char *TAG = "identify";

extern const uint8_t wav_start[] asm("_binary_x_wav_start");
extern const uint8_t wav_end[] asm("_binary_x_wav_end");

static uint32_t rd32(const uint8_t *p) { return p[0] | p[1] << 8 | p[2] << 16 | (uint32_t)p[3] << 24; }
static uint16_t rd16(const uint8_t *p) { return p[0] | p[1] << 8; }

/* walk the RIFF chunks (x.wav has a LIST chunk before "data", so a fixed 44-byte skip is wrong) */
static int wav_find_pcm(const uint8_t *w, size_t size, const int16_t **pcm, size_t *nsamples)
{
    if (size < 12 || memcmp(w, "RIFF", 4) || memcmp(w + 8, "WAVE", 4)) {
        return -1;
    }
    int    fmt_ok = 0;
    size_t off = 12;
    while (off + 8 <= size) {
        const uint8_t *id = w + off;
        uint32_t       len = rd32(w + off + 4);
        const uint8_t *d = w + off + 8;
        if (off + 8 + len > size) {
            len = size - off - 8;
        }
        if (!memcmp(id, "fmt ", 4) && len >= 16) {
            uint16_t fmt = rd16(d), ch = rd16(d + 2), bits = rd16(d + 14);
            uint32_t rate = rd32(d + 4);
            if (fmt != 1 || ch != 1 || rate != AFP_SAMPLE_RATE || bits != 16) {
                ESP_LOGE(TAG, "need 8000 Hz mono 16-bit PCM, got fmt=%u ch=%u rate=%" PRIu32 " bits=%u", fmt, ch,
                         rate, bits);
                return -1;
            }
            fmt_ok = 1;
        } else if (!memcmp(id, "data", 4) && fmt_ok) {
            *pcm = (const int16_t *)d; /* 2-byte aligned: RIFF chunks start at even offsets */
            *nsamples = len / 2;
            return 0;
        }
        off += 8 + len + (len & 1);
    }
    return -1;
}

/* fingerprint of the embedded wav; returns a malloc'ed copy of it */
static void *extract_fp(size_t *fp_len)
{
    const int16_t *pcm;
    size_t         n;
    if (wav_find_pcm(wav_start, wav_end - wav_start, &pcm, &n) != 0) {
        ESP_LOGE(TAG, "bad wav");
        return NULL;
    }
    ESP_LOGI(TAG, "x.wav: %u samples (%.2f s)", (unsigned)n, n / (float)AFP_SAMPLE_RATE);

    size_t              heap0 = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    afp_extr_config_t   cfg = AFP_EXTR_CONFIG_DEFAULT; /* the identify API accepts both is_local_ modes */
    afp_extr_session_t *s = afp_extr_create(&cfg);
    if (s == NULL) {
        return NULL;
    }
    ESP_LOGI(TAG, "session: %u bytes heap", (unsigned)(heap0 - heap_caps_get_free_size(MALLOC_CAP_8BIT)));

    int64_t busy = 0; /* CPU time spent in the extractor */
    const size_t block = 256; /* any block size works; 256 is a typical I2S DMA buffer */
    for (size_t off = 0; off < n; off += block) {
        size_t  c = n - off < block ? n - off : block;
        int64_t t0 = esp_timer_get_time();
        afp_extr_feed(s, pcm + off, c); /* as an I2S reader would deliver it */
        busy += esp_timer_get_time() - t0;
        vTaskDelay(1); /* an I2S read would block here; keeps IDLE / watchdogs alive on single-core chips */
    }
    int64_t t0 = esp_timer_get_time();
    int     ret = afp_extr_finish(s);
    busy += esp_timer_get_time() - t0;

    const void *fp = NULL;
    void       *copy = NULL;
    size_t      len = 0;
    if (ret == 0 && afp_extr_get_fingerprint(s, &fp, &len) == 0 && (copy = malloc(len)) != NULL) {
        memcpy(copy, fp, len);
        *fp_len = len;
        ESP_LOGI(TAG, "fingerprint: %u bytes, extraction CPU %lld ms for %.1f s audio", (unsigned)len,
                 (long long)busy / 1000, n / (float)AFP_SAMPLE_RATE);
    } else {
        ESP_LOGE(TAG, "extraction failed (%d)", ret);
    }
    afp_extr_destroy(s); /* give the ~100 KB back before WiFi/TLS */
    return copy;
}

/* minimal "key":"value" / "key":number lookup, enough for a summary line */
static void json_get(const char *js, const char *key, char *out, size_t size)
{
    char pat[48];
    out[0] = '\0';
    snprintf(pat, sizeof(pat), "\"%s\":", key);
    const char *p = strstr(js, pat);
    if (p == NULL) {
        return;
    }
    p += strlen(pat);
    size_t i = 0;
    if (*p == '"') {
        for (p++; *p && *p != '"' && i + 1 < size; p++) {
            out[i++] = *p;
        }
    } else {
        for (; *p && *p != ',' && *p != '}' && i + 1 < size; p++) {
            out[i++] = *p;
        }
    }
    out[i] = '\0';
}

void app_main(void)
{
    size_t fp_len = 0;
    void  *fp = extract_fp(&fp_len);
    if (fp == NULL) {
        return;
    }

    if (strlen(CONFIG_ACR_ACCESS_KEY) == 0 || strlen(CONFIG_ACR_ACCESS_SECRET) == 0) {
        ESP_LOGE(TAG, "set access_key / access_secret in menuconfig -> ACRCloud identify");
        free(fp);
        return;
    }

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect()); /* WiFi on a board, open_eth in QEMU */

    esp_sntp_config_t sntp = ESP_NETIF_SNTP_DEFAULT_CONFIG(CONFIG_ACR_SNTP_SERVER);
    esp_netif_sntp_init(&sntp);
    if (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(15000)) != ESP_OK) {
        ESP_LOGW(TAG, "SNTP sync timed out, timestamp may be wrong");
    }
    ESP_LOGI(TAG, "time: %lld", (long long)time(NULL));

    const size_t resp_size = 8192;
    char        *resp = malloc(resp_size);
    if (resp == NULL) {
        free(fp);
        return;
    }
    int64_t t0 = esp_timer_get_time();
    int     status = acr_identify(CONFIG_ACR_URL, CONFIG_ACR_ACCESS_KEY, CONFIG_ACR_ACCESS_SECRET, fp, fp_len, resp,
                                  resp_size);
    int64_t t1 = esp_timer_get_time();
    ESP_LOGI(TAG, "HTTP %d, %lld ms, %u bytes response", status, (long long)(t1 - t0) / 1000,
             (unsigned)strlen(resp));
    printf("%s\n", resp);

    char code[16], msg[64], title[128], artist[128], score[16];
    json_get(resp, "code", code, sizeof(code));
    json_get(resp, "msg", msg, sizeof(msg));
    json_get(resp, "title", title, sizeof(title));
    json_get(resp, "artists\":[{\"name", artist, sizeof(artist));
    json_get(resp, "score", score, sizeof(score));
    ESP_LOGI(TAG, "RESULT code=%s msg=%s title=%s artist=%s score=%s", code, msg, title, artist, score);
    ESP_LOGI(TAG, "min free heap %u", (unsigned)heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT));

    free(resp);
    free(fp);
}
