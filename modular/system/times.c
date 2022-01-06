#include "times.h"

#if defined(_WIN32) && defined(_MSC_VER)

//
// usleep - 微秒级别等待函数
// usec     : 等待的微秒
// return   : 0 on success.  On error, -1 is returned.
//
int
usleep(unsigned usec) {
    int ret = -1;
    // Convert to 100 nanosecond interval, negative value indicates relative time
    LARGE_INTEGER t = { .QuadPart = -10LL * usec };

    HANDLE timer = CreateWaitableTimer(NULL, TRUE, NULL);
    if (timer) {
        // 负数以100ns为单位等待, 正数以标准FILETIME格式时间
        SetWaitableTimer(timer, &t, 0, NULL, NULL, FALSE);
        WaitForSingleObject(timer, INFINITE);
        if (GetLastError() == ERROR_SUCCESS)
            ret = 0;
        CloseHandle(timer);
    }

    return ret;
}

#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL

//
// gettimeofday - 实现 Linux sys/time.h 得到微秒时间
// tv       : 返回秒数和微秒数
// tz       : 返回时区结构
// return   : success is 0
//
int 
gettimeofday(struct timeval * restrict tv, struct timezone * restrict tz) {
    if (tv) {
        FILETIME t;
        GetSystemTimeAsFileTime(&t);

        uint64_t m = (uint64_t)t.dwHighDateTime << 32 | t.dwLowDateTime;
        // convert into microseconds, converting file time to unix epoch
        m = m / 10 - DELTA_EPOCH_IN_MICROSECS;

        tv->tv_sec  = (long)(m / 1000000UL);
        tv->tv_usec = (long)(m % 1000000UL);
    }

    // 不管 linux or window, gettimeofday 都不是个好的 api 设计
    if (tz) {
        tz->tz_minuteswest = _timezone / 60;
        tz->tz_dsttime = _daylight;
    }

    return 0;
}

#endif

/* This is a safe version of localtime() which contains no locks and is
 * fork() friendly. Even the _r version of localtime() cannot be used safely
 * in Redis. Another thread may be calling localtime() while the main thread
 * forks(). Later when the child process calls localtime() again, for instance
 * in order to log something to the Redis log, it may deadlock: in the copy
 * of the address space of the forked process the lock will never be released.
 *
 * This function takes the timezone 'tz' as argument, and the 'dst' flag is
 * used to check if daylight saving time is currently in effect. The caller
 * of this function should obtain such information calling tzset() ASAP in the
 * main() function to obtain the timezone offset from the 'timezone' global
 * variable. To obtain the daylight information, if it is currently active or not,
 * one trick is to call localtime() in main() ASAP as well, and get the
 * information from the tm_isdst field of the tm structure. However the daylight
 * time may switch in the future for long running processes, so this information
 * should be refreshed at safe times.
 *
 * Note that this function does not work for dates < 1/1/1970, it is solely
 * designed to work with what time(NULL) may return, and to support Redis
 * logging of the dates, it's not really a complete implementation. */
void 
localtime_get(struct tm * restrict p, time_t t) {
    t -= timezone_get();                /* Adjust for timezone. */
    t += 3600 * daylight_get();         /* Adjust for daylight time. */
    time_t days = t / (3600 * 24);      /* Days passed since epoch. */
    time_t seconds = t % (3600 * 24);   /* Remaining seconds. */

    p->tm_isdst = daylight_get();
    p->tm_hour = seconds / 3600;
    p->tm_min = (seconds % 3600) / 60;
    p->tm_sec = (seconds % 3600) % 60;

    /* 1/1/1970 was a Thursday, that is, day 4 from the POV of the tm structure
     * where sunday = 0, so to calculate the day of the week we have to add 4
     * and take the modulo by 7. */
    p->tm_wday = (days + 4) % 7;

    /* Calculate the current year. */
    p->tm_year = 1970;
    for (;;) {
        /* Leap years have one day more. */
        time_t days_this_year = 365 + is_leap_year(p->tm_year);
        if (days_this_year > days) break;
        days -= days_this_year;
        p->tm_year++;
    }
    p->tm_yday = days;  /* Number of day of the current year. */

    /* We need to calculate in which month and day of the month we are. To do
     * so we need to skip days according to how many days there are in each
     * month, and adjust for the leap year that has one more day in February. */
    int mdays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    mdays[1] += is_leap_year(p->tm_year);

    p->tm_mon = 0;
    while (days >= mdays[p->tm_mon]) {
        days -= mdays[p->tm_mon];
        p->tm_mon++;
    }

    p->tm_mday = days + 1; /* Add 1 since our 'days' is zero-based. */
    p->tm_year -= 1900;    /* Surprisingly tm_year is year-1900. */
}

// times_tm - 从时间串中提取出来年月日时分秒
bool times_tm(times_t ns, struct tm * om) {
    int c, num, * es, * py;
    if ((!ns) || !(c = *ns) || c < '0' || c > '9')
        return false;

    num = 0;
    es = &om->tm_sec;
    py = &om->tm_year;
    do {
        if (c >= '0' && c <= '9') {
            num = 10 * num + c - '0';
            c = *++ns;
            continue;
        }

        *py-- = num;
        if (py < es)
            break;

        // 去掉特殊字符, 重新开始
        for (;;) {
            if ((c = *++ns) == '\0')
                return false;
            if (c >= '0' && c <= '9')
                break;
        }
        num = 0;
    } while (c);

    // true : py < es || c == '\0' && py == es
    if (py < es) return true;
    if (py == es) {
        *es = num;
        return true;
    }
    return false;
}

//
// times_get - 解析时间串, 返回时间戳
// ns       : 时间串内容 
// ot       : 返回得到的时间戳
// om       : 返回得到的时间结构体
// return   : 返回 true 表示构造成功
//
bool
times_get(times_t ns, time_t * ot, struct tm * om) {
    time_t t;
    struct tm m;

    // 先高效解析出年月日时分秒
    if (!times_tm(ns, &m))
        return false;

    // 得到时间戳, 失败返回false
    m.tm_mon -= 1;
    m.tm_year -= 1900;
    if ((t = mktime(&m)) < 0)
        return false;

    // 返回最终结果
    if (ot) *ot = t;
    if (om) *om = m;
    return true;
}

//
// time_get - 解析时间串, 返回时间戳
// ns       : 时间串内容  
// return   : < 0 is error
//
inline time_t
time_get(times_t ns) {
    struct tm m;
    // 先高效解析出年月日时分秒
    if (!times_tm(ns, &m))
        return -1;
    // 得到时间戳, 失败返回false
    m.tm_mon -= 1;
    m.tm_year -= 1900;
    return mktime(&m);
}

//
// time_day - 判断时间戳是否是同一天
// n        : 第一个时间戳
// t        : 第二个时间戳
// return   : true 表示同一天
//
inline bool
time_day(time_t n, time_t t) {
    // China local 适用, 得到当前天数
    // GMT [World] + 8 * 3600 = CST [China]
    n = (n + 8UL * 3600) / (24 * 3600);
    t = (t + 8UL * 3600) / (24 * 3600);
    return n == t;
}

//
// time_week - 判断时间戳是否是同一周
// n        : 第一个时间戳
// t        : 第二个时间戳
// return   : true 表示同一周
//
bool
time_week(time_t n, time_t t) {
    time_t p;
    struct tm m;
    // 获取最大时间存在 n 中
    if (n < t) {
        p = n; n = t; t = p;
    }

    // 得到 n 表示的当前时间
    localtime_r(&n, &m);
    // 得到当前时间到周一起点的时间差
    m.tm_wday = m.tm_wday ? m.tm_wday - 1 : 6;
    p = (time_t)m.tm_wday * 24 * 3600 
      + (time_t)m.tm_hour * 3600 
      + (time_t)m.tm_min * 60 
      + m.tm_sec;

    // [min, n], n = max(n, t) 表示在同一周内
    return t >= n - p;
}

//
// times_day - 判断时间串是否是同一天
// ns       : 第一个时间串
// ts       : 第二个时间串
// return   : true 表示同一天
//
bool
times_day(times_t ns, times_t ts) {
    time_t t, n = time_get(ns);
    // 解析失败直接返回结果
    if ((n < 0) || ((t = time_get(ts)) < 0))
        return false;
    return time_day(n, t);
}

//
// times_week - 判断时间串是否是同一周
// ns       : 第一个时间串
// ts       : 第二个时间串
// return   : true 表示同一周
//
bool
times_week(times_t ns, times_t ts) {
    time_t t, n = time_get(ns);
    // 解析失败直接返回结果
    if ((n < 0) || ((t = time_get(ts)) < 0))
        return false;
    return time_week(n, t);
}

//
// times_fmt - 通过 fmt 格式最终拼接一个字符串
// fmt      : 推荐遵循 TIMES_STR 意图
// out      : 最终保存的内容
// sz       : buf 长度
// return   : 返回生成串长度
//
int
times_fmt(const char * fmt, char out[], size_t sz) {
    struct tm m;
    struct timespec s;

    timespec_get(&s, TIME_UTC);
    localtime_get(&m, s.tv_sec);

    return snprintf(out, sz, fmt,
                    m.tm_year + 1900, m.tm_mon + 1, m.tm_mday,
                    m.tm_hour, m.tm_min, m.tm_sec,
                    (int)(s.tv_nsec / 1000000));
}
