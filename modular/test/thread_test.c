﻿#include <base.h>

static volatile int c;

static void heoo(int index) {
    for (int i = 0; i < 10000; ++i)
        ++c;
    printf("index = %2d, g = %d\n", index, c);
}

//
// test thread demo
//
void thread_test(void) {
    int i, j;
    pthread_t ids[10];

    // 开始运行
    int n = LEN(ids);
    for (i = 0; i < n; ++i)
        if (pthread_run(ids + i, heoo, (void *)(intptr_t)i))
            RETNIL("pthread_run is error!");

    // 二楞子
    pthread_async(heoo, (void *)(intptr_t)-1);

    // 等待结束
    for (j = 0; j < i; ++j)
        pthread_end(ids[j]);

    printf("g = %d\n", c);
}
