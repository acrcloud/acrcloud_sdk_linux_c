#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "acrcloud_recognizer.h"

int main(int argc, char **argv) 
{
    if (!acr_recognize_global_init()) {
        printf("acrcloud init error\n");
        exit(1);
    }

    acrcloud_config config;
    memset(&config, 0, sizeof(acrcloud_config));
    config.host_ = "XXXXXXm";
    config.access_key_ = "XXXXXX";
    config.access_secret_ = "XXXXX";

    config.timeout_ms_ = 5000;
    //config.rec_type_ = acr_opt_rec_humming;
    config.rec_type_ = acr_opt_rec_audio;

    char *result = NULL;
    int result_len = 0;
    // file path, you can also use "acr_recognize_by_filebuffer" function, please refer to  acrcloud_recognizer.h
    acr_recognize_by_file(config, argv[1], 0, &result, &result_len);
    if (result != NULL) {
        printf("** result: %s\n", result);
        acr_free(result);
    }

    // global destroy
    acr_recognize_global_cleanup();
}
