#include <mq.h>

static void echo(void * node) {
    static int idx;

    printf("%d -> %s\n", ++idx, (char *)node);
}

// 消息队列测试
void mq_test(void) {
   struct mq q;
   if (q_init(&q.q)) {
       q.lock = (atomic_flag) ATOMIC_FLAG_INIT;
   }

    mq_push(&q, "123");
    mq_push(&q, "234");
    mq_push(&q, "345");
    mq_push(&q, "456");

    const char * n = mq_pop(&q);
    if (n == nullptr)
        printf("mq is empty! q = %p \n", &q);
    else
        printf("mq_pop = %s\n", n);

    q_delete(&q.q, (node_f)echo);
}
