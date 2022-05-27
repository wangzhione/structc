#include <base.h>

static atomic_int id = ATOMIC_VAR_INIT(1);

//
// EXTERN_TEST  - 单元测试宏, 并打印执行时间
//                常量 CLOCKS_PER_SEC, 它用来表示一秒钟会有多少个时钟计时单元
//                clock_t __cdecl clock(void) 该程序从启动到函数调用占用 CPU 的时间
// ftest        : 测试函数
// ...          : 可变参数
//
#define EXTERN_TEST(ftest, ...)                                                     \
do {                                                                                \
  int $id = atomic_fetch_add(&id, 1);                                               \
  printf("\n[test %d]. "STR(ftest)" run start ...\n\n", $id);                       \
  clock_t $start = clock();                                                         \
  extern void ftest (); ftest (__VA_ARGS__);                                        \
  clock_t $end = clock() - $start;                                                  \
  printf("\n[test %d]. "STR(ftest)" run end difference clock %ld, time %lfms\n\n",  \
         $id, $end, (double)$end / (CLOCKS_PER_SEC / 1000));                        \
} while(0)

//
// main     - run test
//            test build: make test
// return   : exit EXIT_SUCCESS
//
int main(int argc, char * argv[]) {
    // 
    // 系统初始化 ~ 虎
    EXTERN_RUN(init, argc, argv);

    puts("*--------------------------------** main test *--------------------------------*");

    // 
    // unit test ~ 豹
    EXTERN_RUN(test);

    puts("*--------------------------------** main test *--------------------------------*");

    exit(EXIT_SUCCESS);
}

//
// make test 单元测试 show time 开始你的表演
//
void test(void) {
    EXTERN_TEST(stack_test);
    EXTERN_TEST(times_test);
}
