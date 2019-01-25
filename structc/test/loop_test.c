#include <loop.h>
#include <head.h>

#define SLEEP_INT       (1000)

// run 一直写数据
inline static void run(char * msg) {
    static int cnt = 0;
    printf("%d = %s.\n", ++cnt, msg);
}

// 需要处理的函数操作, 写数据进去
static void run_write(loop_t s) {
    int i;
    char * msg;

    // 写五次数据
    for (i = 0; i < 5; ++i) {
        msg = malloc(sizeof(int));
        msg[0] = i + '0';
        msg[1] = '\0';
        loop_push(s, msg);
    }

    msleep(SLEEP_INT);

    // 再写五次数据
    for (i = 5; i < 10; ++i) {
        msg = malloc(sizeof(int));
        msg[0] = i + '0';
        msg[1] = '\0';
        loop_push(s, msg);
    }

    msleep(SLEEP_INT);
}

//
// loop test run .
void loop_test(void) {
    pthread_t id;
    loop_t s = loop_create(run, free);

    // 线程开启, 跑起来测试
    IF(pthread_run(&id, run_write, s));
    // 线程等待
    pthread_end(id);

    loop_delete(s);
}
