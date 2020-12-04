#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

#include "dll_acr_extr_tool.h"
#include "recognizer.h"
#include "http.h"
#include "errorcode.h"
#include "log.h"

void _do_recognize(acrcloud_config config,
            char *ext_fp, int ext_fp_len,
            char *hum_fp, int hum_fp_len,
            char **result, int *result_len);

void acr_rec_set_debug(char is_debug)
{
    if (is_debug) {
        acr_set_debug();
        set_log_level(LOG_DBG);
    } else {
        set_log_level(LOG_SHT);
    }
}

int acr_recognize_global_init() {
    acr_init();

    LOGDBG("acrcloud begin\n");
    if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
        LOGWAR("init error\n");
        return 0;
    }
    
    curl_version_info_data *info = curl_version_info(CURLVERSION_NOW);
    if (info->features & CURL_VERSION_ASYNCHDNS) {
        LOGDBG("c-ares enable\n");
    } else {
        LOGDBG("c-ares disable\n");
    }
    return 1;
}

void acr_recognize_global_cleanup() {
    LOGDBG("acrcloud end\n");
    curl_global_cleanup();
}

void acr_recognize_by_file(acrcloud_config config, 
            char* file_path, int start_time_seconds, 
            char** result, int* result_len) {
    if (config.access_secret_ == NULL || strlen(config.access_secret_) > 512
            || config.access_key_ == NULL || strlen(config.access_key_) > 512
            || file_path == NULL) {
        LOGWAR("access_key access_secret pcm_buffer_len error\n");
        return;
    }

    char *ext_fp = NULL;
    int ext_fp_len = 0;
    char *hum_fp = NULL;
    int hum_fp_len = 0;
    switch (config.rec_type_) {
    case acr_opt_rec_audio:
        ext_fp_len = create_fingerprint_by_file(file_path, start_time_seconds, 10, 0, &ext_fp);
        break;
    case acr_opt_rec_humming:
        hum_fp_len = create_humming_fingerprint_by_file(file_path, start_time_seconds, 20, &hum_fp);
        break;
    case acr_opt_rec_both:
        ext_fp_len = create_fingerprint_by_file(file_path, start_time_seconds, 10, 0, &ext_fp);
        hum_fp_len = create_humming_fingerprint_by_file(file_path, start_time_seconds, 20, &hum_fp);
        break;
    default:
        LOGWAR("config.rec_type_ error\n");
        return;
    }

    if (ext_fp == NULL && hum_fp == NULL) {
        if (ext_fp_len == -1 || hum_fp_len == -1) {
            *result = strdup(AUDIO_DECODE_ERROR);
            *result_len = strlen(AUDIO_DECODE_ERROR); 
            return;
        }
        *result = strdup(CREATE_FINGERPRINT_ERROR);
        *result_len = strlen(CREATE_FINGERPRINT_ERROR);
        return;
    }

    _do_recognize(config, ext_fp, ext_fp_len, hum_fp, hum_fp_len, result, result_len);

    acr_free(ext_fp);
    acr_free(hum_fp);
}

void acr_recognize_by_filebuffer(acrcloud_config config, char* file_buffer, int file_buffer_len, int start_time_seconds, char** result, int* result_len) {
    if (config.access_secret_ == NULL || strlen(config.access_secret_) > 512
            || config.access_key_ == NULL || strlen(config.access_key_) > 512
            || file_buffer == NULL || file_buffer_len <= 0) {
        LOGWAR("access_key access_secret file_buffer_len error\n");
        return;
    }

    char *ext_fp = NULL;
    int ext_fp_len = 0;
    char *hum_fp = NULL;
    int hum_fp_len = 0;
    switch (config.rec_type_) {
    case acr_opt_rec_audio:
        ext_fp_len = create_fingerprint_by_filebuffer(file_buffer, file_buffer_len, start_time_seconds, 10, 0, &ext_fp);
        break;
    case acr_opt_rec_humming:
        hum_fp_len = create_humming_fingerprint_by_filebuffer(file_buffer, file_buffer_len, start_time_seconds, 20, &hum_fp);
        break;
    case acr_opt_rec_both:
        ext_fp_len = create_fingerprint_by_filebuffer(file_buffer, file_buffer_len, start_time_seconds, 10, 0, &ext_fp);
        hum_fp_len = create_humming_fingerprint_by_filebuffer(file_buffer, file_buffer_len, start_time_seconds, 20, &hum_fp);
        break;
    default:
        LOGWAR("config.rec_type_ error\n");
        return;
    }

    if (ext_fp == NULL && hum_fp == NULL) {
        if (ext_fp_len == -1 || hum_fp_len == -1) {
            *result = strdup(AUDIO_DECODE_ERROR);
            *result_len = strlen(AUDIO_DECODE_ERROR); 
            return;
        }
        *result = strdup(CREATE_FINGERPRINT_ERROR);
        *result_len = strlen(CREATE_FINGERPRINT_ERROR);
        return;
    }

    _do_recognize(config, ext_fp, ext_fp_len, hum_fp, hum_fp_len, result, result_len);

    acr_free(ext_fp);
    acr_free(hum_fp);
}

void _do_recognize(acrcloud_config config,
            char *ext_fp, int ext_fp_len,
            char *hum_fp, int hum_fp_len,
            char **result, int *result_len) {
    *result = NULL;
    *result_len = 0;
    if (config.access_secret_ == NULL || strlen(config.access_secret_) > 512
            || config.access_key_ == NULL || strlen(config.access_key_) > 512) {
        return;
    }
    
    LOGDBG("ext_fp_len=%d,hum_fp_len=%d\n", ext_fp_len, hum_fp_len);
    if (ext_fp == NULL && hum_fp == NULL) {
        *result = strdup(NO_RESULT);
        *result_len = strlen(NO_RESULT);
        return;
    }

    acrcloud_http_key_value_node_t nodes[20];
    memset(nodes, 0, 20 * sizeof(acrcloud_http_key_value_node_t));
    char timestamp[40] = {0};
    int t = time(NULL);
    sprintf(timestamp, "%d", t);
    
    char sample_bytes[40] = {0};
    sprintf(sample_bytes, "%d", ext_fp_len);
    char sample_hum_bytes[40] = {0};
    sprintf(sample_hum_bytes, "%d", hum_fp_len);
    
    char signature_src[2048] = {0};
    memset(signature_src, 0, 2048);
    sprintf(signature_src, "POST\n/v1/identify\n%s\nfingerprint\n1\n%d", config.access_key_, t);
    char signature_res[1024] = {0};
    memset(signature_res, 0, 1024);
    gen_signature(signature_src, config.access_secret_, signature_res);

    char* params[][2] = {
        {"access_key", config.access_key_},
        {"timestamp", timestamp},
        {"signature", signature_res},
        {"data_type", "fingerprint"},
        {"signature_version", "1"},
        {NULL, NULL},
    };
    
    int nodes_num = 0;
    while (1) {
        if (params[nodes_num][0] == NULL) break;
        LOGDBG("%s, %s\n", params[nodes_num][0], params[nodes_num][1]);
        nodes[nodes_num].key_ = params[nodes_num][0];
        nodes[nodes_num].value_ = params[nodes_num][1];
        nodes_num++;
    }

    if (ext_fp_len > 0) {
        nodes[nodes_num].key_ = "sample_bytes";
        nodes[nodes_num].value_ = sample_bytes;
        nodes_num++;
        
        nodes[nodes_num].key_ = "sample";
        nodes[nodes_num].value_ = ext_fp;
        nodes[nodes_num].value_len_ = ext_fp_len;
        nodes[nodes_num].type_ = 1;
        nodes_num++;
    }

    if (hum_fp_len > 0) {
        nodes[nodes_num].key_ = "sample_hum_bytes";
        nodes[nodes_num].value_ = sample_hum_bytes;
        nodes_num++;
        
        nodes[nodes_num].key_ = "sample_hum";
        nodes[nodes_num].value_ = hum_fp;
        nodes[nodes_num].value_len_ = hum_fp_len;
        nodes[nodes_num].type_ = 1;
        nodes_num++;
    }

    post(&config, "/v1/identify", nodes, nodes_num, result, result_len);
}

void acr_rec_free(void* buffer) {
    if (buffer != NULL) {
        free(buffer);
    }
}

