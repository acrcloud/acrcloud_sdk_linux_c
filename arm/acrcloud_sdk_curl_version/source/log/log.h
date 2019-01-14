/******************************************************************************************************
*******************************************************************************************************
*/
#define MR_SERVICE_ID        25001

#ifndef __LOG_H__
#define __LOG_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
//#include <sys/slog.h>
//#include <sys/slogcodes.h>

/*begin you must modify this value before you use*/
#define			LOG_MAJOR_ID	MR_SERVICE_ID
#define			LOG_PREFIX		"_MRSERVICE_"
//#define			LOG_OUTPUT_CONSOLE	1//if you want printf log to console.define this.
#define			LOG_OUTPUT	0
/*end you must modify this value before you use*/

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
	LOG_SHT = 0,	/*log shutdown*/
	LOG_CRI	= 1,	/*log critical*/
    LOG_ERR = 2,	/*log error */
    LOG_WAR	= 3,    /*log warning*/
    LOG_NOT	= 4,    /*log notify */
    LOG_INF	= 5,	/*log info*/
    LOG_DBG	= 6,    /*log debug*/
    LOG_LAST,
}LOG_LEV_EN;

extern LOG_LEV_EN GLOBAL_LOG_LEVEL;

void LOGBASE(LOG_LEV_EN level,const char *pcFunc,const char * fmt, ...);
void set_log_level(LOG_LEV_EN log_l);

#ifdef __cplusplus
}
#endif


#endif
