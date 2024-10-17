#ifndef _Included_DLL_acr_extr_tool_H
#define _Included_DLL_acr_extr_tool_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ACR_API
#define ACR_API __attribute__((visibility("default")))
#endif

/**
 *  create audio fingerprint from pcm buffer
 *
 *  @param pcm_buffer: audio data buffer(RIFF (little-endian) data, WAVE audio, Microsoft PCM, 16
 *bit, mono 8000 Hz);
 *  @param pcm_buffer_len: bytes of pcm_buffer
 *  @param is_db_fingerprint: If it is True, it will create db frigerprint (Fingerprint for bucket,
 *not for recognition);
 *
 *  @param fps_buffer: return fingerprint, need "acr_free" to free this memory.
 **/
ACR_API int create_fingerprint(char  *pcm_buffer,
                               int    pcm_buffer_len,
                               char   is_db_fingerprint,
                               char **fps_buffer);

/**
 *  create fingerprint from file path.
 *  Audio: mp3, wav, m4a, flac, aac, amr, ape, ogg ...
 *  Video: mp4, mkv, wmv, flv, ts, avi ...
 *
 *  @param file_path: file path
 *  @param start_time_seconds: skip (start_time_seconds) seconds from from the beginning of
 *(file_path)
 *  @param audio_len_seconds: Length of audio data you need. if you create recogize frigerprint,
 *default is 12 seconds, if you create db frigerprint, it is not usefully;
 *  @param is_db_fingerprint: If it is True, it will create db frigerprint (Fingerprint for bucket,
 *not for recognition);
 *
 *  @param fps_buffer: return fingerprint, need "acr_free" to free this memory.
 **/
ACR_API int create_fingerprint_by_file(char  *file_path,
                                       int    start_time_seconds,
                                       int    audio_len_seconds,
                                       char   is_db_fingerprint,
                                       char **fps_buffer);

/**
 *  create fingerprint from file buffer.
 *  Audio: mp3, wav, m4a, flac, aac, amr, ape, ogg ...
 *  Video: mp4, mkv, wmv, flv, ts, avi ...
 *
 *  @param file_buffer: file buffer
 *  @param file_buffer_len: length of file_buffer
 *  @param start_time_seconds: skip (start_time_seconds) seconds from from the beginning of
 *(file_path)
 *  @param audio_len_seconds: Length of audio data you need. if you create recogize frigerprint,
 *default is 12 seconds, if you create db frigerprint, it is not usefully;
 *  @param is_db_fingerprint: If it is True, it will create db frigerprint (Fingerprint for bucket,
 *not for recognition);
 *
 *  @param fps_buffer: return fingerprint, need "acr_free" to free this memory.
 **/
ACR_API int create_fingerprint_by_filebuffer(char  *file_buffer,
                                             int    file_buffer_len,
                                             int    start_time_seconds,
                                             int    audio_len_seconds,
                                             char   is_db_fingerprint,
                                             char **fps_buffer);

ACR_API int create_fingerprint_by_fpbuffer(char *fp_buffer, int fp_buffer_len, int start_time_seconds, int len_seconds, char **fps_buffer);

/**
 *  create humming fingerprint from file path.
 *  Audio: mp3, wav, m4a, flac, aac, amr, ape, ogg ...
 *  Video: mp4, mkv, wmv, flv, ts, avi ...
 *
 *  @param file_path: file path
 *  @param start_time_seconds: skip (start_time_seconds) seconds from from the beginning of
 *(file_path)
 *  @param audio_len_seconds: Length of audio data you need. if you create recogize frigerprint,
 *default is 12 seconds;
 *
 *  @param fps_buffer: return humming fingerprint, need "acr_free" to free this memory.
 **/
ACR_API int create_humming_fingerprint_by_file(char  *file_path,
                                               int    start_time_seconds,
                                               int    audio_len_seconds,
                                               char **fps_buffer);

/**
 *  create humming fingerprint from file buffer.
 *  Audio: mp3, wav, m4a, flac, aac, amr, ape, ogg ...
 *  Video: mp4, mkv, wmv, flv, ts, avi ...
 *
 *  @param file_buffer: file buffer
 *  @param file_buffer_len: length of file_buffer
 *  @param start_time_seconds: skip (start_time_seconds) seconds from from the beginning of
 *(file_path)
 *  @param audio_len_seconds: Length of audio data you need. if you create recogize frigerprint,
 *default is 12 seconds;
 *
 *  @param fps_buffer: return fingerprint, need "acr_free" to free this memory.
 **/
ACR_API int create_humming_fingerprint_by_filebuffer(char  *file_buffer,
                                                     int    file_buffer_len,
                                                     int    start_time_seconds,
                                                     int    audio_len_seconds,
                                                     char **fps_buffer);

/**
 *  decode audio buffer (8000hz mono 16bit) from file path.
 *  Audio: mp3, wav, m4a, flac, aac, amr, ape, ogg ...
 *  Video: mp4, mkv, wmv, flv, ts, avi ...
 *
 *  @param file_path: file path
 *  @param start_time_seconds: skip (start_time_seconds) seconds from from the beginning of
 *(file_path)
 *  @param audio_len_seconds: Length of audio data you need. if you create recogize frigerprint,
 *default is 12 seconds;
 *
 *  @param audio_buffer: return audio buffer, need "acr_free" to free this memory.
 **/
ACR_API int decode_audio_by_file(char  *file_path,
                                 int    start_time_seconds,
                                 int    audio_len_seconds,
                                 char **audio_buffer);

/**
 *  decode audio buffer (8000hz mono 16bit) from file buffer.
 *  Audio: mp3, wav, m4a, flac, aac, amr, ape, ogg ...
 *  Video: mp4, mkv, wmv, flv, ts, avi ...
 *
 *  @param file_buffer: file buffer
 *  @param file_buffer_len: bytes of file buffer
 *  @param start_time_seconds: skip (start_time_seconds) seconds from from the beginning of
 *(file_path)
 *  @param audio_len_seconds: Length of audio data you need. if you create recogize frigerprint,
 *default is 12 seconds;
 *
 *  @param audio_buffer: return audio buffer, need "acr_free" to free this memory.
 **/
ACR_API int decode_audio_by_filebuffer(char  *file_buffer,
                                       int    file_buffer_len,
                                       int    start_time_seconds,
                                       int    audio_len_seconds,
                                       char **audio_buffer);

ACR_API int get_duration_ms_by_file(char *file_path);
ACR_API int get_duration_ms_by_filebuffer(char *file_buffer, int file_buffer_len);
ACR_API int get_duration_ms_by_fpbuffer(char *fp_buffer, int fp_buffer_len);

ACR_API void acr_free(char *buffer);
ACR_API void acr_set_debug();
ACR_API void acr_init();

#ifdef __cplusplus
}
#endif

#endif
