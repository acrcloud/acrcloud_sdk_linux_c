#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>

#include "acrcloud_http.h"
#include "../exception/acrcloud_errorcode.h"
#include "../log/log.h"

#define CURL_RESULT_BUFFER_BLOCK_LEN (1024)

struct curl_result_callback_s {
	char *buffer_;
	int buffer_max_len_;
	int buffer_len_;
};
typedef struct curl_result_callback_s curl_result_callback_t;

size_t _process_curl_result_data(void *buffer, size_t size, size_t nmemb,
		void *userp) {
	LOGDBG("%s %d %d\n", __func__, size, nmemb);
	curl_result_callback_t *t_result = (curl_result_callback_t*) userp;
	if (t_result->buffer_len_ + size * nmemb > t_result->buffer_max_len_) {
		t_result->buffer_max_len_ += CURL_RESULT_BUFFER_BLOCK_LEN
				* (size * nmemb / CURL_RESULT_BUFFER_BLOCK_LEN + 1);
		LOGDBG("%s realloc %d\n", __func__, t_result->buffer_max_len_);
		t_result->buffer_ = realloc(t_result->buffer_,
				t_result->buffer_max_len_);
		if (t_result->buffer_ == NULL) {
			LOGWAR("%s realloc len=%d error\n", __func__, t_result->buffer_max_len_);
			t_result->buffer_max_len_ = 0;
			t_result->buffer_len_ = 0;
			return 0;
		}
		memset(t_result->buffer_ + t_result->buffer_len_, 0,
				t_result->buffer_max_len_ - t_result->buffer_len_);
	}
	memcpy(t_result->buffer_ + t_result->buffer_len_, buffer, size * nmemb);
	t_result->buffer_len_ += size * nmemb;
	return size * nmemb;
}

void post(const char* host, short port, const char* http_path_file,
		acrcloud_http_key_value_node_t *nodes, int nodes_num, char **result,
		int *result_len, int timeout_ms) {
	CURL *curl = NULL;
	CURLcode curl_code = CURLE_OK;

	curl_result_callback_t t_result;
	memset(&t_result, 0, sizeof(curl_result_callback_t));
	t_result.buffer_max_len_ = CURL_RESULT_BUFFER_BLOCK_LEN * 2;
	t_result.buffer_ = (char*) malloc(t_result.buffer_max_len_);
	memset(t_result.buffer_, 0, t_result.buffer_max_len_);
	if (t_result.buffer_ == NULL) {
		LOGWAR("%s malloc len=%d error\n", __func__, t_result.buffer_max_len_);
		*result = NULL;
		*result_len = 0;
		goto error;
	}

	curl = curl_easy_init();
	if (curl == NULL) {
		LOGWAR("%s curl init error\n", __func__);
		*result = strdup(HTTP_CURL_ERROR);
		*result_len = strlen(HTTP_CURL_ERROR);
		goto error;
	}

	char acr_url[2048];
	sprintf(acr_url, "http://%s%s", host, http_path_file);
	LOGDBG("%s\n", acr_url);

	struct curl_httppost *formpost = NULL;
	struct curl_httppost *lastptr = NULL;

	int i = 0;
	for (i = 0; i < nodes_num; i++) {
		if (nodes[i].type_ == 1) {
			LOGDBG("%s : %d\n", nodes[i].key_, nodes[i].value_len_);
			curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, nodes[i].key_,
					CURLFORM_BUFFER, nodes[i].key_, CURLFORM_BUFFERPTR,
					nodes[i].value_, CURLFORM_BUFFERLENGTH,
					(long) nodes[i].value_len_, CURLFORM_END);
		} else {
			LOGDBG("%s : %s\n", nodes[i].key_, nodes[i].value_);
			curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, nodes[i].key_,
					CURLFORM_PTRCONTENTS, nodes[i].value_, CURLFORM_END);
		}
	}

	curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
	curl_easy_setopt(curl, CURLOPT_URL, acr_url);
	//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout_ms);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, timeout_ms);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _process_curl_result_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &t_result);
	curl_code = curl_easy_perform(curl);
	if (curl_code != CURLE_OK) {
		LOGWAR("%s curl_code=%d error\n", __func__, curl_code);
		if (curl_code == CURLE_OPERATION_TIMEOUTED) {
			*result = strdup(HTTP_TIMEOUT_ERROR);
			*result_len = strlen(HTTP_TIMEOUT_ERROR);
		} else {
			*result = strdup(HTTP_CURL_ERROR);
			*result_len = strlen(HTTP_CURL_ERROR);
		}
		goto error;
	}

	LOGDBG("%s result : %s\n", __func__, t_result.buffer_);
	LOGDBG("%s\n", __func__);

	while (t_result.buffer_len_ > 1) {
		if (t_result.buffer_[t_result.buffer_len_ - 1] == '}')
			break;
		t_result.buffer_len_--;
	}
	if (t_result.buffer_[0] != '{'
			|| t_result.buffer_[t_result.buffer_len_ - 1] != '}') {
		LOGWAR("%s result=%s json error\n", __func__, t_result.buffer_);
		*result = strdup(JSON_ERROR);
		*result_len = strlen(JSON_ERROR);
		goto error;
	}

	*result = t_result.buffer_;
	*result_len = t_result.buffer_len_;
	goto end;

error:
	free(t_result.buffer_);
	memset(&t_result, 0, sizeof(curl_result_callback_t));

end:
	curl_formfree(formpost);
	curl_easy_cleanup(curl);
}

