#ifndef _H_TIMES
#define _H_TIMES

#include <time.h>
#include <stdbool.h>

//
// 1s = 1000ms  = 1000000us  = 1000000000ns
// 1秒  1000毫秒  1000000微秒  1000000000纳秒
// ~ 力求最小时间业务单元 ~ 
//

#ifdef __GUNC__

#include <unistd.h>
#include <sys/time.h>

//
// msleep - 睡眠函数, 颗粒度是毫秒.
// m        : 待睡眠的毫秒数
// return   : void
//
inline void msleep(int ms) {
    usleep(ms * 1000);
}

#endif

#ifdef _MSC_VER

#include <windows.h>

inline void msleep(int ms) {
    Sleep(ms);
}


//
// localtime_r - 安全的得到当前时间结构体
// timep        : 输入的时间戳指针
// result       : 返回输出时间结构
// return       : 失败 NULL, 正常返回 result
//
inline struct tm * localtime_r(const time_t * timep, struct tm * result) {
    return localtime_s(result, timep) ? NULL : result;
}

//
// usleep - 微秒级别等待函数
// usec         : 等待的微秒
// return       : 0 on success.  On error, -1 is returned.
//
extern int usleep(unsigned usec);

#endif

// times_t - 时间串类型
#define INT_TIMES (64)
typedef char times_t[INT_TIMES];

#endif//_H_TIMES
