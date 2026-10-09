#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "mbedtls/base64.h"
#include "mbedtls/md.h"

#include "acr_identify.h"

static const char *TAG = "acr";

#define BOUNDARY "----afpextr7d1a9c3e5b"

typedef struct {
    char  *buf;
    size_t size;
    size_t len;
} resp_buf_t;

static esp_err_t on_http_event(esp_http_client_event_t *evt)
{
    resp_buf_t *r = (resp_buf_t *)evt->user_data;
    if (evt->event_id == HTTP_EVENT_ON_DATA && r->len + 1 < r->size) {
        size_t n = evt->data_len;
        if (n > r->size - 1 - r->len) {
            n = r->size - 1 - r->len;
        }
        memcpy(r->buf + r->len, evt->data, n);
        r->len += n;
        r->buf[r->len] = '\0';
    }
    return ESP_OK;
}

/* path part of the URL, e.g. "/v1/identify" (it is part of the string to sign) */
static const char *url_path(const char *url)
{
    const char *p = strstr(url, "://");
    p = p ? p + 3 : url;
    p = strchr(p, '/');
    return p ? p : "/";
}

static int sign_request(const char *access_secret, const char *string_to_sign, char *out, size_t out_size)
{
    unsigned char mac[20];
    size_t        olen = 0;

    if (mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA1), (const unsigned char *)access_secret,
                        strlen(access_secret), (const unsigned char *)string_to_sign, strlen(string_to_sign),
                        mac) != 0) {
        return -1;
    }
    if (mbedtls_base64_encode((unsigned char *)out, out_size - 1, &olen, mac, sizeof(mac)) != 0) {
        return -1;
    }
    out[olen] = '\0';
    return 0;
}

static size_t add_field(char *p, size_t room, const char *name, const char *value)
{
    int n = snprintf(p, room, "--" BOUNDARY "\r\nContent-Disposition: form-data; name=\"%s\"\r\n\r\n%s\r\n", name,
                     value);
    return (n > 0 && (size_t)n < room) ? (size_t)n : room;
}

int acr_identify(const char *url, const char *access_key, const char *access_secret, const void *fp, size_t fp_len,
                 char *resp, size_t resp_size)
{
    const char *data_type = "fingerprint";
    const char *sig_version = "1";
    char        timestamp[24], sample_bytes[16], signature[48];
    char        string_to_sign[256];
    int         status = -1;

    snprintf(timestamp, sizeof(timestamp), "%lld", (long long)time(NULL));
    snprintf(sample_bytes, sizeof(sample_bytes), "%u", (unsigned)fp_len);
    snprintf(string_to_sign, sizeof(string_to_sign), "POST\n%s\n%s\n%s\n%s\n%s", url_path(url), access_key, data_type,
             sig_version, timestamp);
    if (sign_request(access_secret, string_to_sign, signature, sizeof(signature)) != 0) {
        ESP_LOGE(TAG, "signing failed");
        return -1;
    }

    /* multipart/form-data body: 6 text fields + the fingerprint as file "sample" */
    const size_t head_room = 1024;
    const char   tail[] = "\r\n--" BOUNDARY "--\r\n";
    char        *body = malloc(head_room + fp_len + sizeof(tail));
    if (body == NULL) {
        return -1;
    }
    size_t len = 0;
    len += add_field(body + len, head_room - len, "access_key", access_key);
    len += add_field(body + len, head_room - len, "sample_bytes", sample_bytes);
    len += add_field(body + len, head_room - len, "timestamp", timestamp);
    len += add_field(body + len, head_room - len, "signature", signature);
    len += add_field(body + len, head_room - len, "data_type", data_type);
    len += add_field(body + len, head_room - len, "signature_version", sig_version);
    len += snprintf(body + len, head_room - len,
                    "--" BOUNDARY "\r\nContent-Disposition: form-data; name=\"sample\"; filename=\"test.bin\"\r\n"
                    "Content-Type: application/octet-stream\r\n\r\n");
    if (len >= head_room - 1) {
        ESP_LOGE(TAG, "form header too long");
        free(body);
        return -1;
    }
    memcpy(body + len, fp, fp_len);
    len += fp_len;
    memcpy(body + len, tail, sizeof(tail) - 1);
    len += sizeof(tail) - 1;

    resp_buf_t               r = {.buf = resp, .size = resp_size, .len = 0};
    esp_http_client_config_t cfg = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .timeout_ms = 15000,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .event_handler = on_http_event,
        .user_data = &r,
    };
    resp[0] = '\0';
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    if (client == NULL) {
        free(body);
        return -1;
    }
    esp_http_client_set_header(client, "Content-Type", "multipart/form-data; boundary=" BOUNDARY);
    esp_http_client_set_post_field(client, body, (int)len);

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        status = esp_http_client_get_status_code(client);
    } else {
        ESP_LOGE(TAG, "HTTP request failed: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
    free(body);
    return status;
}
