#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "recognizer.h"

int main(int argc, char **argv) 
{
    if (argc != 2) {
        printf("usage: %s audio_video_filepath\n", argv[0]);
        return -1;
    }

    acr_rec_set_debug(1);
    if (!acr_recognize_global_init()) {
        printf("acrcloud init error\n");
        exit(1);
    }

    //FILE* fp_wav = fopen(argv[1], "rb");
    //if (fp_wav == NULL)
    //    return 1;

    //fseek(fp_wav,0L,SEEK_END);
    //int size = ftell(fp_wav);
    //fseek(fp_wav,44,SEEK_SET); // If have wav header.

    //if (size <= 0) {
    //    return 1;
    //}

    //char *pcm_buffer = (char*)malloc(size);
    //int nres = fread(pcm_buffer, 1, size, fp_wav);

    //fclose(fp_wav);

    acrcloud_config config;
    memset(&config, 0, sizeof(acrcloud_config));
    config.host_ = "XXXXXX";
    config.access_key_ = "XXXXXX";
    config.access_secret_ = "XXXXXX";

    config.timeout_ms_ = 5000;
    //config.rec_type_ = acr_opt_rec_humming;
    config.rec_type_ = acr_opt_rec_audio;

    char *result = NULL;
    int result_len = 0;
    acr_recognize_by_file(config, argv[1], 0, &result, &result_len);
    if (result != NULL) {
        printf("** result: %s\n", result);
        acr_rec_free(result);
    }

    acr_recognize_global_cleanup();
}
