#include <head.h>
#include <timer.h>

// one - 简单数据打印
inline static void one(void * arg) {
    static int m;
    times_t str; times_str(str);
    printf("%p | %d => %s\n", arg, ++m, str);
}

// two - 连环施法
inline static void two(void * arg) {
    printf("1s after one arg = %p\n", arg);

    timer_add(1000, one, arg);
}

// timer 定时器测试
void timer_test(void) {
    timer_add(   0, one, (void *)(intptr_t)1);
    timer_add(3000, two, (void *)(intptr_t)2);
    timer_add(4000, one, (void *)(intptr_t)3);

    // 测试一个连环施法
    int id = timer_add(6000, two, (void *)(intptr_t)4);
    IF(id < 0);

    // 等待 5 秒后关闭 上面永久的定时器事件
    msleep(5000);
    timer_del(id);
}
