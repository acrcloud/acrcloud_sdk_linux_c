#ifndef __ACRCLOUD_RECOGNIZER_H
#define __ACRCLOUD_RECOGNIZER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32 //windows platform
 
#ifdef ACR_USER_MODULE_EXPORTS
#define ACR_API __declspec(dllexport)
#else
#define ACR_API __declspec(dllimport)
#endif
 
#ifndef ACR_CALL_TYPE
#define ACR_CALL_TYPE  	__stdcall  
#endif
 
#else //linux platform
 
#ifndef ACR_API
#define ACR_API __attribute__ ((visibility ("default")))
#endif
 
#ifndef ACR_CALL_TYPE
#define ACR_CALL_TYPE
#endif
 
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
    char* device_id_;
    ACRCLOUD_OPT_REC_TYPE rec_type_;
};
typedef struct acrcloud_config_s acrcloud_config;

struct acrcloud_params_s {
    char* key_;
    char* value_;
};
typedef struct acrcloud_params_s acrcloud_params_t;


/**
 *   If you use acr_recognize_by_pcm function, you must call acr_recognize_global_init when your APP initialized.
 *
 *   @return 0 something error, 1 suss
 */
ACR_API int acr_recognize_global_init();

/**
 *   If you call acr_recognize_global_cleanup to use acr_recognize_by_pcm, you must call this function when your APP exits.
 */
ACR_API void acr_recognize_global_cleanup();

/**
 *
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
 *
**/
ACR_API void acr_recognize_by_pcm(acrcloud_config config, char* pcm_buffer, int pcm_buffer_len, int nchannels, int sample_rate, char** result, int* result_len, acrcloud_params_t *user_params, int user_params_size);


/**
 *
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
ACR_API void acr_create_audio_fingerprint_by_pcm(char* pcm_buffer, int pcm_buffer_len, int nchannels, int sample_rate, char** fp_buffer, int* fp_buffer_len);



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
ACR_API void acr_create_humming_fingerprint_by_pcm(char* pcm_buffer, int pcm_buffer_len, int nchannels, int sample_rate, char** fp_buffer, int* fp_buffer_len);



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
ACR_API void acr_recognize_by_8k_pcm(acrcloud_config config, char* pcm_buffer, int pcm_buffer_len, char** result, int* result_len);



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
ACR_API void acr_create_audio_fingerprint_by_8k_pcm(char* pcm_buffer, int pcm_buffer_len, char** fp_buffer, int* fp_buffer_len);

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
ACR_API void acr_create_humming_fingerprint_by_8k_pcm(char* pcm_buffer, int pcm_buffer_len, char** fp_buffer, int* fp_buffer_len);

/**
 *
 *  resample wav audio buffer to (RIFF (little-endian) data, WAVE audio, Microsoft PCM, 16 bit, mono 8000 Hz) 
 *
 *  @param pcm_buffer: query audio buffer[ (little-endian) data, WAVE audio, Microsoft PCM, 16 bit, mono 8000 Hz]
 *  @param pcm_buffer_len: the length of pcm_buffer 
 *  @param fp_buffer: fingerprint of pcm_buffer, you must free this buffer by acr_free.
 *  @param fp_buffer_len: the length of fp_buffer
 *
**/
ACR_API void acr_resample_to_8k_pcm(char* pcm_buffer, int pcm_buffer_len, int nchannels, int sample_rate, char** out_8k_pcm_buffer, int* out_8k_pcm_buffer_len);


/**
 * free buffer that other function return.
**/
ACR_API void acr_free(void* buffer);

/**
 * set debug model.
**/
ACR_API void acr_set_debug(char is_debug);

#ifdef __cplusplus
}
#endif

#endif /* __ACRCLOUD_RECOGNIZER_H */
