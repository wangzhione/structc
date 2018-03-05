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
