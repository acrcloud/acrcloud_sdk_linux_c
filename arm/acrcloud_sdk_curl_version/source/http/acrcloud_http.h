#ifndef __ACRCLOUD_HTTP_H
#define __ACRCLOUD_HTTP_H

//#include "tcp.h"

#ifdef __cplusplus
extern "C" {
#endif

struct acrcloud_http_key_value_node_s {
    char *key_;
    char *value_;
    int value_len_;
    int type_;
};
typedef struct acrcloud_http_key_value_node_s acrcloud_http_key_value_node_t;


void post(const char* host, short port, const char* http_path_file, acrcloud_http_key_value_node_t *nodes, int nodes_num, char **result, int *result_len, int timeout_ms);

#ifdef __cplusplus
}
#endif

#endif /* __ACRCLOUD_HTTP_H */

