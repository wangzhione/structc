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

    time_t now = time_now(t);
    if (now < 0)
        RETNIL("times test error tsr = %s", tsr);

    printf("tsr = %s -> t = %zd\n", tsr, t);
    printf("nsr = %s -> n = %zd\n", nsr, n);

    times_t esr = "2018年03月 :) 19点35分59秒";
    time_t e = time_get(esr);
    if (e < 0)
        printf("esr = %s -> e = %zd\n", esr, e);
}
