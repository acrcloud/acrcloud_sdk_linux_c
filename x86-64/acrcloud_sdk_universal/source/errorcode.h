#ifndef __ACRCLOUD_ERRORCODE_H
#define __ACRCLOUD_ERRORCODE_H

#ifdef __cplusplus
extern "C" {
#endif

static const char* SERVER_RESULT_ERROR = "{\"status\":{\"msg\":\"Http Error, Server Result\",\"code\":3000,\"version\":\"1.0\"}}";
static const char* HTTP_TIMEOUT_ERROR = "{\"status\":{\"msg\":\"Http Timeout\",\"code\":3001,\"version\":\"1.0\"}}";
static const char* HTTP_CURL_ERROR = "{\"status\":{\"msg\":\"Http curl Error\",\"code\":3002,\"version\":\"1.0\"}}";
static const char* CREATE_FINGERPRINT_ERROR = "{\"status\":{\"msg\":\"Create Fingerprint Error\",\"code\":2004,\"version\":\"1.0\"}}";
static const char* AUDIO_DECODE_ERROR = "{\"status\":{\"msg\":\"Audio Decode Error\",\"code\":2005,\"version\":\"1.0\"}}";
static const char* RESAMPLE_ERROR = "{\"status\":{\"msg\":\"Resample Error\",\"code\":2006,\"version\":\"1.0\"}}";
static const char* NO_RESULT = "{\"status\":{\"msg\":\"No Result\",\"code\":1000,\"version\":\"1.0\"}}";
static const char* JSON_ERROR = "{\"status\":{\"msg\":\"Json Error\",\"code\":2002,\"version\":\"1.0\"}}";

#ifdef __cplusplus
}
#endif

#endif /* __ACRCLOUD_ERRORCODE_H */


