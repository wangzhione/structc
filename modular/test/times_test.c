#include <times.h>
#include <struct.h>

//
// time 业务测试
//
void times_test(void) {
    // Now 'timezome' global is populated. Obtain timezone and daylight info. 
    tzset();

    printf("daylight = %d, daylight = %d\n", daylight, daylight);
    printf("daylight = %d, daylight = %d\n", daylight, daylight);

    times_t tsr = "2018年3月11日 19点35分59秒";
    times_t nsr = "2018-03-11 19:35:58";
    time_t t = time_get(tsr);
    time_t n = time_get(nsr);
    printf("t = %lld, n = %lld\n", (long long)t, (long long)n);
    IF(t < 0 || n < 0);

    printf("tsr = %s -> t = %zd\n", tsr, t);
    printf("nsr = %s -> n = %zd\n", nsr, n);

    times_t esr = "2018年03月 :) 19点35分59秒";
    time_t e = time_get(esr);
    if (e < 0)
        printf("esr = %s -> e = %zd\n", esr, e);

    struct timeval tv;
    // struct timezone tz;
    // int status = gettimeofday(&tv, &tz);
    int status = gettimeofday(&tv, nullptr);
    printf("status = %d\n", status);
    printf("tv_sec = %ld, tv_usec = %ld\n", (long)tv.tv_sec, (long)tv.tv_usec);
    // printf("tz_minuteswest = %d, tz_dsttime = %d\n", tz.tz_minuteswest, tz.tz_dsttime);
    printf("timezone = %ld, daylight = %d, tzname=%s%s\n", timezone, daylight, tzname[0], tzname[1]);

    tzset();
    times_t nos;
    time_t now = time(nullptr);

    struct tm * p = localtime(&now);
    strftime(nos, sizeof nos, "%Y/%m/%d %H:%M:%S", p);
    puts(nos);

    struct tm nom;
    localtime_get(&nom, now);
    strftime(nos, sizeof nos, "%Y/%m/%d %H:%M:%S", &nom);
    puts(nos);

    time_t t1 = 1647835351;
    time_t t2 = 1647836351;
    bool equal = time_day_equal(t1, t2);
    printf("equal = %s\n", equal ? "true" : "false");
}

void times_fmt_test(void) {
    times_t buf;
    int len;

    printf("timezone = %ld, daylight = %d, tzname=%s, %s\n", timezone, daylight, tzname[0], tzname[1]);

    len = times_fmt(TIMES_FMT_STR, buf, sizeof buf);
    printf("len = %d, buf = [%s]\n", len, buf);

    char * time_fmt_str = "%04d年%02d月%02d日 %02d时%02d分%02d秒.%09ld纳秒 %s";    
    len = times_fmt(time_fmt_str, buf, sizeof buf);
    printf("len = %d, buf = [%s]\n", len, buf);

    time_fmt_str = "%04d-%02d-%02d %02d:%02d:%02d.%09ld %s";    
    len = times_fmt(time_fmt_str, buf, sizeof buf);
    printf("len = %d, buf = [%s]\n", len, buf);

    printf("times() = %s, len = %zu\n", times(), strlen(times()));
}
