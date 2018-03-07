#include <times.h>

#ifdef _MSC_VER

//
// usleep - 微秒级别等待函数
// usec         : 等待的微秒
// return       : 0 on success.  On error, -1 is returned.
//
int 
usleep(unsigned usec) {
    int rt = -1;
    // Convert to 100 nanosecond interval, negative value indicates relative time
    LARGE_INTEGER ft = { .QuadPart = -10ll * usec };

    HANDLE timer = CreateWaitableTimer(NULL, TRUE, NULL);
    if (timer) {
        // 负数以100ns为单位等待, 正数以标准FILETIME格式时间
        SetWaitableTimer(timer, &ft, 0, NULL, NULL, FALSE);
        WaitForSingleObject(timer, INFINITE);
        if (GetLastError() == ERROR_SUCCESS)
            rt = 0;
        CloseHandle(timer);
    }

    return rt;
}

#endif

// times_tm - 从时间串中提取出来年月日时分秒
extern bool times_tm(times_t tsr, struct tm * pm) {
    int c, num, * es, * py;
    if ((!tsr) || !(c = *tsr) || c < '0' || c > '9')
        return false;

    num = 0;
    es = &pm->tm_sec;
    py = &pm->tm_year;
    do {
        if (c >= '0' && c <= '9') {
            num = 10 * num + c - '0';
            c = *++tsr;
            continue;
        }

        *py-- = num;
        num = 0;

        // 去掉特殊字符, 一直找到下一个数字
        while ((c = *++tsr) && (c < '0' || c > '9'))
            ;
    } while (c && py >= es);
    // 解析失败返回 false,
    if (py != es)
        return false;

    *es = num;
    return true;
}

//
// times_get - 解析时间串, 返回时间戳
// tsr          : 时间串内容 
// pt           : 返回得到的时间戳
// pm           : 返回得到的时间结构体
// return       : 返回 true 表示构造成功
//
bool 
times_get(times_t tsr, time_t * pt, struct tm * pm) {
    time_t t;
    struct tm m;

    // 先高效解析出年月日时分秒
    if (!times_tm(tsr, &m))
        return false;

    // 得到时间戳, 失败返回false
    m.tm_year -= 1900;
    m.tm_mon -= 1;
    if ((t = mktime(&m)) < 0)
        return false;

    // 返回最终结果
    if (pt) *pt = t;
    if (pm) *pm = m;
    return true;
}
