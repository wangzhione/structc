#include <chead.h>

//
// TEST - 用于单元测试函数, 执行并输出运行时间
// ftest    : 需要执行的测试函数名称
// ...      : 可变参数, 保留
//
#define TEST(ftest, ...)                                           \
do {                                                               \
    extern void ftest();                                           \
    clock_t $s = clock();                                          \
    ftest (##__VA_ARGS__);                                         \
    double $e = (double)clock();                                   \
    printf(STR(ftest) " run time:%lfs\n", ($e-$s)/CLOCKS_PER_SEC); \
} while(0)


//
// main_test - *_test is here run
// return   : void
//
void main_test(void) {
    //
    // 开始你的表演, 单元测试
    //

    EXTERN_RUN(pipe_test);
}
