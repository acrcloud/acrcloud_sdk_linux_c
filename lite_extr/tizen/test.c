#include <stdio.h>
#include <stdlib.h>
#include "acrcloud_extr_lite.h"

int main(int argc,char *argv[])
{
    /**** read wav from file *****/
    long   size = 0;
    size_t nres = 0;
    FILE  *fp_wav = fopen(argv[1], "rb");
    if (fp_wav == NULL) {
        fprintf(stderr, "read wav error\n");
        return -1;
    }

    fseek(fp_wav, 0L, SEEK_END);
    size = ftell(fp_wav);
    fseek(fp_wav, 0L, SEEK_SET);
    //int nsamples_ = (size - 44) / 2;
    int8_t * pcm_ = (int8_t *)malloc(size);
    nres = fread(pcm_, sizeof(int8_t), size, fp_wav);
    if (nres != (size_t)size) {
        fclose(fp_wav);
        return -1;
    }
    fclose(fp_wav);


    /*********  create fingerprint from pcm audio buffer ***********/
    char *fp, *fp_hum;
    int fp_len = 0, fp_hum_len = 0;
    acr_create_audio_fingerprint_by_pcm((char *)pcm_, nres, 1, 8000, &fp, &fp_len);
    printf("fp_len=%d\n", fp_len);
    // need to free this memory
    acr_free(fp);

    //acr_create_humming_fingerprint_by_pcm((char *)pcm_, nres, 1, 8000, &fp_hum, &fp_hum_len);
    //acr_free(fp_hum);

    //FILE  *fp_out = fopen("fp.bin", "wb");
    //fwrite(fp, 1, fp_len, fp_out);
    //fclose(fp_out);
}
