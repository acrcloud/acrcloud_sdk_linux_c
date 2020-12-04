#ifndef __ACRCLOUD_RECOGNIZER_H
#define __ACRCLOUD_RECOGNIZER_H

#ifdef __cplusplus
extern "C" {
#endif

enum ACRCLOUD_OPT_REC_TYPE_S {
    acr_opt_rec_audio,
    acr_opt_rec_humming,
    acr_opt_rec_both,
};
typedef enum ACRCLOUD_OPT_REC_TYPE_S ACRCLOUD_OPT_REC_TYPE;

struct acrcloud_config_s {
    char* host_;
    char* access_key_;
    char* access_secret_;
    int timeout_ms_;
    char* curl_proxy_address_;
    int rec_length_seconds_;
    ACRCLOUD_OPT_REC_TYPE rec_type_;
};
typedef struct acrcloud_config_s acrcloud_config;

/**
 *   You must call acr_recognize_global_init when your APP initialized.
 *
 *   @return 0 something error, 1 suss
 */
int acr_recognize_global_init();

/**
 *   If you call acr_recognize_global_cleanup to use acr_recognize_by_pcm, you must call this function when your APP exits.
 */
void acr_recognize_global_cleanup();

/**
 *  recognize by wav audio file path
 *  Audio: mp3, wav, m4a, flac, aac, amr, ape, ogg ...
 *  Video: mp4, mkv, wmv, flv, ts, avi ...
 *
 *  @param config: query config 
 *  @param file_path: query audio file path
 *  @param start_time_seconds: skip (start_time_seconds) seconds from from the beginning of (file_path)
 *  @param result: json result metainfo, you must free this buffer by acr_free.  https://docs.acrcloud.com/metadata
 *  @param result_len: the length of result 
 *
**/
void acr_recognize_by_file(acrcloud_config config, char* file_path, int start_time_seconds, char** result, int* result_len);

/**
 *  recognize by wav audio file buffer 
 *  Audio: mp3, wav, m4a, flac, aac, amr, ape, ogg ...
 *  Video: mp4, mkv, wmv, flv, ts, avi ...
 *
 *  @param config: query config 
 *  @param pcm_buffer: query audio buffer
 *  @param pcm_buffer_len: the length of pcm_buffer
 *  @param result: json result metainfo, you must free this buffer by acr_free.  https://docs.acrcloud.com/metadata
 *  @param result_len: the length of result 
 *
**/
void acr_recognize_by_filebuffer(acrcloud_config config, char* file_buffer, int file_buffer_len, int start_time_seconds, char** result, int* result_len);


/**
 * free buffer that other function return.
**/
void acr_rec_free(void* buffer);

/**
 * set debug model.
**/
void acr_rec_set_debug(char is_debug);

#ifdef __cplusplus
}
#endif

#endif /* __ACRCLOUD_RECOGNIZER_H */

