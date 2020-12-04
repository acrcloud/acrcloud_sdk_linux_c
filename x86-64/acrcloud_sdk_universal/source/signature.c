#include <stdio.h>
#include <string.h>
#include "signature.h"
#include "log.h"

void gen_signature(const char* signature_src, char* access_secret, char* signature_res)
{
    char signature_res_tmp[64];
    hmac_sha1(access_secret, strlen(access_secret), signature_src, strlen(signature_src), signature_res_tmp);
    base64_encode(signature_res_tmp, 20, signature_res, 64);
    LOGDBG("%s\n", signature_res);
}
