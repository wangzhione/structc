#include <thread.h>

static void _heoo(int cnt) {
    for (int i = 0; i < cnt; ++i) {
        printf("%s -> %d\n", __func__, i);
    }
}

//
// test thread demo
//
void thread_test(void) {
    pthread_t tid;
    if (pthread_run(tid, _heoo, __LINE__)) {
        RETNIL("pthread_run is error!");
    }
    pthread_end(tid);
}
