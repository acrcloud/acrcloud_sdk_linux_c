#ifndef _ACRCLOUDSDK_H_
#define _ACRCLOUDSDK_H_

#ifndef EXPORT_API
#define EXPORT_API __attribute__((__visibility__("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 *  create audio fingerprint by wav audio buffer(RIFF (little-endian) data, WAVE audio, Microsoft PCM, 16 bit)
 *
 *  @param pcm_buffer: query audio buffer[ (little-endian) data, WAVE audio, Microsoft PCM, 16 bit]
 *  @param pcm_buffer_len: the length of pcm_buffer
 *  @param nchannels:  channels of pcm_buffer
 *  @param sample_rate: sample rate of pcm_buffer
 *  @param fp_buffer: fingerprint of pcm_buffer, you must free this buffer by acr_free.
 *  @param fp_buffer_len: the length of fp_buffer
 *
 **/
EXPORT_API void acr_create_audio_fingerprint_by_pcm(char* pcm_buffer,
		int pcm_buffer_len, int nchannels, int sample_rate, char** fp_buffer, int* fp_buffer_len);

/**
 *
 *  create humming fingerprint by wav audio buffer(RIFF (little-endian) data, WAVE audio, Microsoft PCM, 16 bit)
 *
 *  @param pcm_buffer: query audio buffer[ (little-endian) data, WAVE audio, Microsoft PCM, 16 bit]
 *  @param pcm_buffer_len: the length of pcm_buffer
 *  @param nchannels:  channels of pcm_buffer
 *  @param sample_rate: sample rate of pcm_buffer
 *  @param fp_buffer: fingerprint of pcm_buffer, you must free this buffer by acr_free.
 *  @param fp_buffer_len: the length of fp_buffer
 *
 **/
EXPORT_API void acr_create_humming_fingerprint_by_pcm(char* pcm_buffer,
		int pcm_buffer_len, int nchannels, int sample_rate, char** fp_buffer, int* fp_buffer_len);

/**
 * free buffer that other function return.
 **/
EXPORT_API void acr_free(void* buffer);

#ifdef __cplusplus
}
#endif
#endif // _ACRCLOUDSDK_H_

