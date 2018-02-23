#include <socket.h>

#ifdef _MSC_VER

//
// gettimeofday - Linux sys/time.h 中得到微秒时间实现
// tv		:	返回结果包含秒数和微秒数
// tz		:	包含的时区, winds 上这个变量没有作用
// return	:   默认返回 0
//
int 
gettimeofday(struct timeval * tv, void * tz) {
    struct tm m;
    SYSTEMTIME se;

    GetLocalTime(&se);
    m.tm_year = se.wYear - 1900;
    m.tm_mon = se.wMonth - 1;
    m.tm_mday = se.wDay;
    m.tm_hour = se.wHour;
    m.tm_min = se.wMinute;
    m.tm_sec = se.wSecond;
    m.tm_isdst = -1; // 不考虑夏令时

    tv->tv_sec = (long)mktime(&m);
    tv->tv_usec = se.wMilliseconds * 1000;

    return 0;
}

#endif
