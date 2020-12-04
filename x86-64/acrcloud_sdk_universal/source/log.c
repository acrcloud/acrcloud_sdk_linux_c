#include "log.h"

LOG_LEV_EN GLOBAL_LOG_LEVEL = LOG_WAR;

void LOGBASE(LOG_LEV_EN level, const char *pcFunc, const char * fmt, ...)
{
    static unsigned int minor = 0;
    char buffer[BUFFSIZE];
    int n = sprintf(buffer,"%s[%s]-", LOG_PREFIX, pcFunc);

    va_list vap;
    va_start(vap, fmt);
    vsnprintf(buffer+n,BUFFSIZE-n, fmt,vap);
    va_end(vap);

    if (level <= GLOBAL_LOG_LEVEL) {
        printf("%04d,level:%02d,%s", minor++, level, buffer);
    }
}

void set_log_level(LOG_LEV_EN log_l)
{
    GLOBAL_LOG_LEVEL = log_l;
}
