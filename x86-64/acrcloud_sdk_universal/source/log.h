#ifndef __LOG_H__
#define __LOG_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>

#define BUFFSIZE 5120
#define LOG_PREFIX "_ACRCLOUD_"

#define     LOGSHT(fmt,args...)   		LOGBASE(LOG_SHT,__FUNCTION__,fmt,## args)
#define     LOGCRI(fmt,args...)   		LOGBASE(LOG_CRI,__FUNCTION__,fmt,## args)
#define     LOGERR(fmt,args...)   		LOGBASE(LOG_ERR,__FUNCTION__,fmt,## args)
#define     LOGWAR(fmt,args...)   		LOGBASE(LOG_WAR,__FUNCTION__,fmt,## args)
#define     LOGNOT(fmt,args...)   		LOGBASE(LOG_NOT,__FUNCTION__,fmt,## args)
#define     LOGINF(fmt,args...)   		LOGBASE(LOG_INF,__FUNCTION__,fmt,## args)
#define     LOGDBG(fmt,args...)   		LOGBASE(LOG_DBG,__FUNCTION__,fmt,## args)
#define     LOGOUT(level,fmt,args...)   LOGBASE(level,__FUNCTION__,fmt,## args)

typedef enum
{
    LOG_SHT = 0,
    LOG_CRI = 1,
    LOG_ERR = 2,
    LOG_WAR = 3,
    LOG_NOT = 4,
    LOG_INF = 5,
    LOG_DBG = 6,
    LOG_LAST,
}LOG_LEV_EN;

extern LOG_LEV_EN GLOBAL_LOG_LEVEL;

void LOGBASE(LOG_LEV_EN level,const char *pcFunc,const char * fmt, ...);
void set_log_level(LOG_LEV_EN log_l);

#ifdef __cplusplus
}
#endif


#endif
