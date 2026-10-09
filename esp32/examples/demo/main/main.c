/*
 * afp_extr demo for ESP32.
 *
 * Generates 10 s of synthetic 8 kHz audio in 256-sample blocks (the size an I2S
 * driver would typically deliver), feeds it to the streaming extractor and
 * reports speed and memory. To use a real microphone, replace synth_block()
 * with i2s_channel_read() + conversion to 8 kHz mono int16.
 */
#include <inttypes.h>
#include <math.h>
#include <stdio.h>

#include "esp_heap_caps.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "afp_extr.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DEMO_SECONDS 10
#define BLOCK        256 /* any block size works; 256 is a typical I2S DMA buffer */

/* a simple "melody": a note change every 250 ms, two harmonics, a bit of noise */
static void synth_block(int16_t *out, int n, uint32_t *t)
{
    static const float notes[] = {262, 294, 330, 349, 392, 440, 494, 523, 587, 659, 698, 784};
    static float       ph1, ph2, ph3;
    static uint32_t    lcg = 1;

    for (int i = 0; i < n; i++, (*t)++) {
        float f = notes[(*t / 2000 * 7) % (sizeof(notes) / sizeof(notes[0]))];
        ph1 += 2.0f * (float)M_PI * f / AFP_SAMPLE_RATE;
        ph2 += 2.0f * (float)M_PI * 2.0f * f / AFP_SAMPLE_RATE;
        ph3 += 2.0f * (float)M_PI * 3.0f * f / AFP_SAMPLE_RATE;
        if (ph1 > 2.0f * (float)M_PI) ph1 -= 2.0f * (float)M_PI;
        if (ph2 > 2.0f * (float)M_PI) ph2 -= 2.0f * (float)M_PI;
        if (ph3 > 2.0f * (float)M_PI) ph3 -= 2.0f * (float)M_PI;
        lcg = lcg * 1664525u + 1013904223u;
        float noise = (float)((int32_t)(lcg >> 16) - 32768) / 32768.0f;
        out[i] = (int16_t)(8000.0f * sinf(ph1) + 4000.0f * sinf(ph2) + 2000.0f * sinf(ph3) + 500.0f * noise);
    }
}

static void afp_task(void *arg)
{
    (void)arg;
    int16_t  block[BLOCK];
    uint32_t t = 0;

    size_t heap0 = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    printf("free heap %u, largest block %u\n", (unsigned)heap0,
           (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));

    afp_extr_config_t   cfg = AFP_EXTR_CONFIG_DEFAULT; /* change fields here if needed */
    afp_extr_session_t *s = afp_extr_create(&cfg);
    if (s == NULL) {
        printf("afp_extr_create failed\n");
        vTaskDelete(NULL);
        return;
    }
    printf("session uses %u bytes of heap\n", (unsigned)(heap0 - heap_caps_get_free_size(MALLOC_CAP_8BIT)));

    int64_t busy = 0, worst = 0;
    int     nblocks = DEMO_SECONDS * AFP_SAMPLE_RATE / BLOCK;
    for (int b = 0; b < nblocks; b++) {
        synth_block(block, BLOCK, &t);
        int64_t t0 = esp_timer_get_time();
        if (afp_extr_feed(s, block, BLOCK) != 0) {
            printf("feed failed\n");
            break;
        }
        int64_t dt = esp_timer_get_time() - t0;
        busy += dt;
        if (dt > worst) worst = dt;
        /* a real I2S read blocks while waiting for data; yield the same way so that
         * single-core chips (C3/C6/...) keep IDLE and the watchdogs running */
        vTaskDelay(1);
    }
    int64_t t0 = esp_timer_get_time();
    int     ret = afp_extr_finish(s);
    int64_t t_finish = esp_timer_get_time() - t0;

    printf("audio %d s: feed %" PRId64 " ms total, worst block %" PRId64 " us (budget %d us), finish %" PRId64
           " us\n",
           DEMO_SECONDS, busy / 1000, worst, BLOCK * 1000000 / AFP_SAMPLE_RATE, t_finish);
    printf("real-time factor %.3f (CPU load on one core)\n", busy / 1e6 / DEMO_SECONDS);

    const void *fp = NULL;
    size_t      fp_len = 0;
    if (ret == 0 && afp_extr_get_fingerprint(s, &fp, &fp_len) == 0) {
        printf("fingerprint: %u bytes (library %s)\n", (unsigned)fp_len, afp_extr_version());
    } else {
        printf("no fingerprint (ret=%d)\n", ret);
    }

    printf("min free heap %u, task stack high-water %u bytes\n",
           (unsigned)heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT),
           (unsigned)(uxTaskGetStackHighWaterMark(NULL) * sizeof(StackType_t)));

    afp_extr_destroy(s);
    vTaskDelete(NULL);
}

void app_main(void)
{
    xTaskCreatePinnedToCore(afp_task, "afp", 6144, NULL, 5, NULL, tskNO_AFFINITY);
}
