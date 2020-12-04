#ifndef __ACRCLOUD_HTTP_H
#define __ACRCLOUD_HTTP_H

#include "recognizer.h"

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

void post(acrcloud_config* mconfig, 
              const char* http_endpoint, 
              acrcloud_http_key_value_node_t *nodes, 
              int nodes_num, char **result, int *result_len);

#ifdef __cplusplus
}
#endif

#endif /* __ACRCLOUD_HTTP_H */

