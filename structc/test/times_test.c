#include <times.h>
#include <struct.h>

//
// time 业务测试
//
void times_test(void) {
    times_t tsr = "2018年3月11日 19点35分59秒";
    times_t nsr = "2018-03-11 19:35:58";
    time_t t = time_get(tsr);
    time_t n = time_get(nsr);
    IF(t < 0 || n < 0);

    printf("tsr = %s -> t = %zd\n", tsr, t);
    printf("nsr = %s -> n = %zd\n", nsr, n);

    times_t esr = "2018年03月 :) 19点35分59秒";
    time_t e = time_get(esr);
    if (e < 0)
        printf("esr = %s -> e = %zd\n", esr, e);

    struct timeval tv;
    struct timezone tz;
    int status = gettimeofday(&tv, &tz);
    printf("status = %d\n", status);
    printf("tv_sec = %ld, tv_usec = %ld\n", (long)tv.tv_sec, (long)tv.tv_usec);
    printf("tz_minuteswest = %d, tz_dsttime = %d\n", tz.tz_minuteswest, tz.tz_dsttime);
    printf("timezone = %ld, daylight = 0\n", timezone_get());

    tzset();
    times_t nos;
    time_t now = time(NULL);

    struct tm * p = localtime(&now);
    strftime(nos, sizeof nos, "%Y/%m/%d %H:%M:%S", p);
    puts(nos);

    struct tm nom;
    localtime_get(&nom, now);
    strftime(nos, sizeof nos, "%Y/%m/%d %H:%M:%S", &nom);
    puts(nos);
}
