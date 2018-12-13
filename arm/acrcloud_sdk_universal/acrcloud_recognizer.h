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
 *  recognize by wav audio buffer(RIFF (little-endian) data, WAVE audio, Microsoft PCM, 16 bit) 
 *
 *  @param config: query config 
 *  @param pcm_buffer: query audio buffer[ (little-endian) data, WAVE audio, Microsoft PCM, 16 bit]
 *  @param nchannels:  channels
 *  @param sample_rate: sample rate of pcm_buffer
 *  @param pcm_buffer_len: the length of pcm_buffer 
 *  @param result: json result metainfo, you must free this buffer by acr_free.  https://docs.acrcloud.com/metadata
 *  @param result_len: the length of result 
 *
**/
void acr_recognize_by_pcmbuffer(acrcloud_config config, char* pcm_buffer, int pcm_buffer_len, int nchannels, int sample_rate, char** result, int* result_len);

/**
  *  create "ACRCloud Fingerprint" by file path of (Audio/Video file)
  *          Audio: mp3, wav, m4a, flac, aac, amr, ape, ogg ...
  *          Video: mp4, mkv, wmv, flv, ts, avi ...
  *
  *  @param file_path query file path
  *  @param skip (startSeconds) seconds from from the beginning of (filePath)
  *  @param audio_len_seconds Length of audio data you need. if you create recogize frigerprint, default is 12 seconds, if you create db frigerprint, it is not usefully; 
  *  @param is_db_fingerprint If it is True, it will create db frigerprint; 
  *  @param fps_buffer "ACRCloud Fingerprint"
  *  
  *  @return The length of fps_buffer "ACRCloud Fingerprint"
**/
int acr_create_fingerprint_by_file(char *file_path, int start_time_seconds, int audio_len_seconds, char is_db_fingerprint, char **fps_buffer);

/**
  *
  *  create "ACRCloud Fingerprint" by file buffer of (Audio/Video file)
  *          Audio: mp3, wav, m4a, flac, aac, amr, ape, ogg ...
  *          Video: mp4, mkv, wmv, flv, ts, avi ...
  *
  *  @param file_buffer data buffer of input file
  *  @param file_buffer_len length of fileBuffer
  *  @param start_time_seconds skip (startSeconds) seconds from from the beginning of (filePath)
  *  @param audio_len_seconds Length of audio data you need. if you create recogize frigerprint, default is 12 seconds, if you create db frigerprint, it is not usefully; 
  *  @param is_db_fingerprint If it is True, it will create db frigerprint; 
  *  @param fps_buffer "ACRCloud Fingerprint"; 
  *  
  *  @return The lenght of fps_buffer "ACRCloud Fingerprint"
**/
int acr_create_fingerprint_by_filebuffer(char *file_buffer, int file_buffer_len, int start_time_seconds, int audio_len_seconds, char is_db_fingerprint, char **fps_buffer);

/**
 *  create audio fingerprint by wav audio buffer(RIFF (little-endian) data, WAVE audio, Microsoft PCM, 16 bit, mono 8000 Hz) 
 *
 *  @param pcm_buffer: query audio buffer[ (little-endian) data, WAVE audio, Microsoft PCM, 16 bit]
 *  @param pcm_buffer_len: the length of pcm_buffer 
 *  @param nchannels:  channels
 *  @param sample_rate: sample rate of pcm_buffer
 *  @param fp_buffer: fingerprint of pcm_buffer, you must free this buffer by acr_free.
 *  @param fp_buffer_len: the length of fp_buffer
 *
**/
void acr_create_fingerprint_by_pcmbuffer(char* pcm_buffer, int pcm_buffer_len, int nchannels, int sample_rate, char** fp_buffer, int* fp_buffer_len);

/**
 *
 *  create humming fingerprint by wav audio buffer(RIFF (little-endian) data, WAVE audio, Microsoft PCM, 16 bit) 
 *
 *  @param pcm_buffer: query audio buffer[ (little-endian) data, WAVE audio, Microsoft PCM, 16 bit]
 *  @param pcm_buffer_len: the length of pcm_buffer 
 *  @param nchannels:  channels
 *  @param sample_rate: sample rate of pcm_buffer
 *  @param fp_buffer: fingerprint of pcm_buffer, you must free this buffer by acr_free. 
 *  @param fp_buffer_len: the length of fp_buffer
 *
**/
void acr_create_humming_fingerprint_by_pcmbuffer(char* pcm_buffer, int pcm_buffer_len, int nchannels, int sample_rate, char** fp_buffer, int* fp_buffer_len);

/**
 * free buffer that other function return.
**/
void acr_free(void* buffer);

/**
 * set debug model.
**/
void acr_set_debug(char is_debug);

#ifdef __cplusplus
}
#endif

#endif /* __ACRCLOUD_RECOGNIZER_H */

