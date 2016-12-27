# Audio&Humming Recognition Linux C SDK

## Overview
  [ACRCloud](https://www.acrcloud.com/) provides [Automatic Content Recognition](https://www.acrcloud.com/docs/introduction/automatic-content-recognition/) services for [Audio Fingerprinting](https://www.acrcloud.com/docs/introduction/audio-fingerprinting/) based applications such as **[Audio Recognition](https://www.acrcloud.com/music-recognition)** (supports music, video, ads for both online and offline), **[Broadcast Monitoring](https://www.acrcloud.com/broadcast-monitoring)**, **[Second Screen](https://www.acrcloud.com/second-screen-synchronization)**, **[Copyright Protection](https://www.acrcloud.com/copyright-protection-de-duplication)** and etc.<br>
  
  This **audio&humming recognition Linux C SDK** only support RIFF (little-endian) data, WAVE audio, Microsoft PCM, 16 bit, mono 8000 Hz)

## Requirements
Follow one of the tutorials to create a project and get your host, access_key and access_secret.

 * [How to identify songs by sound](https://www.acrcloud.com/docs/tutorials/identify-music-by-sound/)
 
 * [How to detect custom audio content by sound](https://www.acrcloud.com/docs/tutorials/identify-audio-custom-content/)
 
## Functions
Introduction all API.
### recognizer.cs
```c
 /**
 *
 *  recognize by wav audio buffer(RIFF (little-endian) data, WAVE audio, Microsoft PCM, 16 bit, mono 8000 Hz) 
 *
 *  @param config: query config 
 *  @param pcm_buffer: query audio buffer[ (little-endian) data, WAVE audio, Microsoft PCM, 16 bit, mono 8000 Hz]
 *  @param pcm_buffer_len: the length of pcm_buffer 
 *  @param result: json result metainfo, you must free this buffer by acr_free.  https://docs.acrcloud.com/metadata
 *  @param result_len: the length of result 
 *  
 *
**/
void acr_recognize_by_8k_pcm(acrcloud_config config, char* pcm_buffer, int pcm_buffer_len, char** result, int* result_len);

/**
 *
 *  create audio fingerprint by wav audio buffer(RIFF (little-endian) data, WAVE audio, Microsoft PCM, 16 bit, mono 8000 Hz) 
 *
 *  @param pcm_buffer: query audio buffer[ (little-endian) data, WAVE audio, Microsoft PCM, 16 bit, mono 8000 Hz]
 *  @param pcm_buffer_len: the length of pcm_buffer 
 *  @param fp_buffer: fingerprint of pcm_buffer, you must free this buffer by acr_free.
 *  @param fp_buffer_len: the length of fp_buffer
 *
**/
void acr_create_audio_fingerprint_by_8k_pcm(char* pcm_buffer, int pcm_buffer_len, char** fp_buffer, int* fp_buffer_len);

/**
 *
 *  create humming fingerprint by wav audio buffer(RIFF (little-endian) data, WAVE audio, Microsoft PCM, 16 bit, mono 8000 Hz) 
 *
 *  @param pcm_buffer: query audio buffer[ (little-endian) data, WAVE audio, Microsoft PCM, 16 bit, mono 8000 Hz]
 *  @param pcm_buffer_len: the length of pcm_buffer 
 *  @param fp_buffer: fingerprint of pcm_buffer, you must free this buffer by acr_free. 
 *  @param fp_buffer_len: the length of fp_buffer
 *
**/
void acr_create_humming_fingerprint_by_8k_pcm(char* pcm_buffer, int pcm_buffer_len, char** fp_buffer, int* fp_buffer_len);

/**
 * free buffer that other function return.
**/
void acr_free(void* buffer);
 
```

## Example
You need to replace "XXXXXXXX" below with your project's host, access_key and access_secret, and run it.
```c
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
    acr_recognize_by_8k_pcm(config, pcm_buffer, nres, &result, &result_len);
    if (result != NULL) {
        printf("result: %s\n", result);
        acr_free(result);
    }


    // Create audio fingerprint;
    /*char *audio_fp = NULL;
    int audio_fp_len = 0;
    acr_create_audio_fingerprint_by_8k_pcm(pcm_buffer, nres, &audio_fp, &audio_fp_len);
    if (audio_fp != NULL) {
        printf("fp len: %d\n", audio_fp_len);
        acr_free(audio_fp);
    }*/

    // Create humming fingerprint
    /*char *humming_fp = NULL;
    int humming_fp_len = 0;
    acr_create_audio_fingerprint_by_8k_pcm(pcm_buffer, nres, &humming_fp, &humming_fp_len);
    if (audio_fp != NULL) {
        printf("fp len: %d\n", humming_fp_len);
        acr_free(humming_fp);
    }*/

```
