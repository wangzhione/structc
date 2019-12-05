#include "base.h"

//
// EXTERN_TEST  - 单元测试宏, 并打印执行时间
// ftest        : 测试函数
// ...          : 可变参数
//
#define EXTERN_TEST(ftest, ...)                              \
do {                                                         \
    clock_t $s = clock();                                    \
    extern void ftest ();                                    \
    ftest ( __VA_ARGS__);                                    \
    double $e = (double)clock();                             \
    printf(STR(ftest)" run %lfs\n", ($e-$s)/CLOCKS_PER_SEC); \
} while(0)

//
// test     - main run test
// return   : void
//
void test(void) {
    puts("*--------------------------------** main test *--------------------------------*");

    //
    // 单元测试, 开始你的表演 show time
    //
    EXTERN_RUN(times_test);

    puts("*--------------------------------** main test *--------------------------------*");
}
