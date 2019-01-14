#ifndef __ACRCLOUD_ERRORCODE_H
#define __ACRCLOUD_ERRORCODE_H

#ifdef __cplusplus
extern "C" {
#endif

static const char* SERVER_RESULT_ERROR = "{\"status\":{\"msg\":\"http error, server result\",\"code\":3000,\"version\":\"1.0\"}}";
static const char* HTTP_TIMEOUT_ERROR = "{\"status\":{\"msg\":\"http timeout\",\"code\":2005,\"version\":\"1.0\"}}";
static const char* HTTP_CURL_ERROR = "{\"status\":{\"msg\":\"http curl error\",\"code\":2007,\"version\":\"1.0\"}}";
static const char* CREATE_FINGERPRINT_ERROR = "{\"status\":{\"msg\":\"create fingerprint error\",\"code\":2004,\"version\":\"1.0\"}}";
static const char* RESAMPLE_ERROR = "{\"status\":{\"msg\":\"resample error\",\"code\":2006,\"version\":\"1.0\"}}";
static const char* NO_RESULT = "{\"status\":{\"msg\":\"No Result\",\"code\":1000,\"version\":\"1.0\"}}";
static const char* JSON_ERROR = "{\"status\":{\"msg\":\"Json error\",\"code\":2002,\"version\":\"1.0\"}}";

#ifdef __cplusplus
}
#endif

#endif /* __ACRCLOUD_ERRORCODE_H */


