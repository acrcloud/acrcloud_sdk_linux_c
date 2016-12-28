#include <stdio.h>
#include <stdlib.h>
#include "acrcloud_recognizer.h"

int main(int argc, char **argv) 
{
    if (argc != 4) { 
        printf("arg error: %s file nchannels sample_rate\n", argv[0]);
        return 0;
    }
    int nchannels = atoi(argv[2]);
    int sample_rate = atoi(argv[3]);
    FILE* fp_wav = fopen(argv[1], "rb");

    if (fp_wav == NULL)
        return 1;

    fseek(fp_wav,0L,SEEK_END);
    int size = ftell(fp_wav);
    fseek(fp_wav,44,SEEK_SET); // If have wav header.

    if (size <= 0) {
        return 1;
    }

    char *pcm_buffer = (char*)malloc(size);
    int nres = fread(pcm_buffer, 1, size, fp_wav);

    fclose(fp_wav);

    acrcloud_config config;
    config.host_ = "<your host>";
    config.access_key_ = "<your access_key>";
    config.access_secret_ = "<your access_secret>";
    config.timeout_ms_ = 5000;
    config.rec_type_ = acr_opt_rec_humming; // you can change it [acr_opt_rec_audio, acr_opt_rec_humming, acr_opt_rec_both].

    char *result = NULL;
    int result_len = 0;
    acr_recognize_by_pcm(config, pcm_buffer, nres, nchannels, sample_rate, &result, &result_len);
    if (result != NULL) {
        printf("result: %s\n", result);
        acr_free(result);
    }

    // Resample 
    /*char *out_pcm_buffer = NULL;
    int out_pcm_buffer_len = 0;
    acr_resample_to_8k_pcm(pcm_buffer, nres, nchannels, sample_rate, &out_pcm_buffer, &out_pcm_buffer_len);
    printf("%d\n", out_pcm_buffer_len);*/


    // Create audio fingerprint;
    /*char *audio_fp = NULL;
    int audio_fp_len = 0;
    acr_create_audio_fingerprint_by_pcm(pcm_buffer, nres, nchannels, sample_rate, &audio_fp, &audio_fp_len);
    if (audio_fp != NULL) {
        printf("fp len: %d\n", audio_fp_len);
        acr_free(audio_fp);
    }*/

    // Create humming fingerprint
    /*char *humming_fp = NULL;
    int humming_fp_len = 0;
    acr_create_audio_fingerprint_by_pcm(pcm_buffer, nres, nchannels, sample_rate, &humming_fp, &humming_fp_len);
    if (audio_fp != NULL) {
        printf("fp len: %d\n", humming_fp_len);
        acr_free(humming_fp);
    }*/
}
