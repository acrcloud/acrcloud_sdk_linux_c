#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

#include "acrcloud_recognizer.h"
#include "acrcloud_extr.h"
#include "http/acrcloud_http.h"
#include "crypto/acrcloud_signature.h"
#include "exception/acrcloud_errorcode.h"
#include "utils/utils.h"
#include "log/log.h"

ACR_API void acr_set_debug(char is_debug)
{
	if (is_debug) {
        set_log_level(LOG_DBG);
	} else {
        set_log_level(LOG_WAR);
	}
}

ACR_API int acr_recognize_global_init() {
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

ACR_API void acr_recognize_global_cleanup() {
	LOGDBG("acrcloud end\n");
	curl_global_cleanup();
}

ACR_API void acr_recognize_by_pcm(acrcloud_config config, char* pcm_buffer,
		int pcm_buffer_len, int nchannels, int sample_rate, char** result, int* result_len) {
	*result = NULL;
	*result_len = 0;

    if (config.access_secret_ == NULL || strlen(config.access_secret_) > 512
			|| config.access_key_ == NULL || strlen(config.access_key_) > 512
			|| pcm_buffer == NULL || pcm_buffer_len <= 0) {
		LOGWAR("access_keyaccess_secret pcm_buffer_len error\n");
        if (config.access_secret_ == NULL) printf("config.access_secret_ == NULL\n");
        else printf("config.access_secret_ %s\n", config.access_secret_);
        if (config.access_key_ == NULL) printf("config.access_key_ == NULL\n");
        else printf("config.access_key_ %s\n", config.access_key_);
        if (pcm_buffer == NULL) printf("pcm_buffer == NULL\n");
        printf("pcm_buffer_len = %d\n", pcm_buffer_len);
		return;
	}

	char *ext_fp = NULL;
	int ext_fp_len = 0;
	char *hum_fp = NULL;
	int hum_fp_len = 0;
	switch (config.rec_type_) {
	case acr_opt_rec_audio:
		acr_create_audio_fingerprint_from_pcm(pcm_buffer, pcm_buffer_len, nchannels, sample_rate,
				&ext_fp, &ext_fp_len, 0);
		break;
	case acr_opt_rec_humming:
		acr_create_humming_fingerprint_from_pcm(pcm_buffer, pcm_buffer_len, nchannels, sample_rate,
				&hum_fp, &hum_fp_len, 0);
		break;
	case acr_opt_rec_both:
        acr_create_audio_fingerprint_from_pcm(pcm_buffer, pcm_buffer_len, nchannels, sample_rate,
				&ext_fp, &ext_fp_len, 0);
        acr_create_humming_fingerprint_from_pcm(pcm_buffer, pcm_buffer_len, nchannels, sample_rate,
				&hum_fp, &hum_fp_len, 0);
		break;
	default:
		LOGWAR("config.rec_type_ error\n");
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
	char timestamp[40];
	int t = time(NULL);
	sprintf(timestamp, "%d", t);

	char sample_bytes[40];
	sprintf(sample_bytes, "%d", ext_fp_len);
	char sample_hum_bytes[40];
	sprintf(sample_hum_bytes, "%d", hum_fp_len);

	char signature_src[2048];
	memset(signature_src, 0, 2048);
	sprintf(signature_src, "POST\n/v1/identify\n%s\nfingerprint\n1\n%d",
			config.access_key_, t);
	char signature_res[1024];
	memset(signature_res, 0, 1024);

	char local_mac[128] = { 0 };
	memset(local_mac, 0, 128);
	if (get_mac(local_mac) <= 0) {
		sprintf(local_mac, "%s", "NOMAC");
	}

	acr_create_signature(signature_src, config.access_secret_, signature_res);
	nodes[0].key_ = "access_key";
	nodes[0].value_ = config.access_key_;

	nodes[1].key_ = "timestamp";
	nodes[1].value_ = timestamp;

	nodes[2].key_ = "signature";
	nodes[2].value_ = signature_res;

	nodes[3].key_ = "data_type";
	nodes[3].value_ = "fingerprint";

	nodes[4].key_ = "signature_version";
	nodes[4].value_ = "1";

	nodes[5].key_ = "uuid";
	nodes[5].value_ = local_mac;

	int ii = 0;
	for (ii = 0; ii < 6; ii++) {
		LOGDBG("%s, %s\n", nodes[ii].key_, nodes[ii].value_);
	}

	int nodes_num = 6;
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

	post(config.host_, 80, "/v1/identify", nodes, nodes_num, result, result_len,
			config.timeout_ms_);

	acr_extr_free(ext_fp);
	acr_extr_free(hum_fp);
}

