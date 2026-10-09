#ifndef _ACR_IDENTIFY_H
#define _ACR_IDENTIFY_H

#include <stddef.h>

/*
 * ACRCloud Identify Protocol V1, data_type = "fingerprint".
 *
 * fp:   the bytes returned by afp_extr_get_fingerprint(), sent unchanged.
 * resp: receives the JSON response (NUL-terminated, truncated to resp_size - 1).
 * Returns the HTTP status code, or -1 on a local / network error.
 * Needs the system clock set (SNTP): the timestamp is part of the signature.
 */
int acr_identify(const char *url, const char *access_key, const char *access_secret, const void *fp, size_t fp_len,
                 char *resp, size_t resp_size);

#endif
