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
extern bool times_tm(times_t tsr, struct tm * ptm) {
    int c, num, * es, * py;
    if ((!tsr) || !(c = *tsr) || c < '0' || c > '9')
        return false;

    num = 0;
    es = &ptm->tm_sec;
    py = &ptm->tm_year;
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

    // 解析成功返回 true, 否则 false
    if (py == es) {
        *es = num;
        return true;
    }
    return false;
}
