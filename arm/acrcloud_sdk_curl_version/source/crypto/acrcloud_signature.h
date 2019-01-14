#ifndef __ACRCLOUD_SIGNATURE_H
#define __ACRCLOUD_SIGNATURE_H

#ifdef __cplusplus
extern "C" {
#endif

void acr_create_signature(const char* signature_src, char* access_secret, char* signature_res);

#ifdef __cplusplus
}
#endif

#endif /* __ACRCLOUD_SIGNATURE_H */

