#define Q_INT     (1<< 5)
#include <q.h>

#undef Q_INT
#define Q_INT     (1<< 5)

struct object {
    int value;
};

// queue 功能单元测试
void q_test(void) {
    int i;
    struct object object[1000];

    for (i = 0; i < (int)(sizeof object / sizeof *object); i++) {
        object[i].value = i;
    }

    struct q q = q_create();
    
    printf("i = %2d, q_len = %2d, q->tail=%2d, q->head=%2d, q->cap=%2d\n", i, q_len(&q), q.tail, q.head, q.cap);
    i = 0; q_push(&q, object+i);
    printf("i = %2d, q_len = %2d, q->tail=%2d, q->head=%2d, q->cap=%2d\n", i, q_len(&q), q.tail, q.head, q.cap);
    i = 1; q_push(&q, object+i);
    printf("i = %2d, q_len = %2d, q->tail=%2d, q->head=%2d, q->cap=%2d\n", i, q_len(&q), q.tail, q.head, q.cap);
    q_pop(&q);
    printf("i = %2d, q_len = %2d, q->tail=%2d, q->head=%2d, q->cap=%2d\n", i, q_len(&q), q.tail, q.head, q.cap);

    // 开始填充
    for (i = 0; i < 40; i++) {
        q_push(&q, object+i);
    printf("i = %2d, q_len = %2d, q->tail=%2d, q->head=%2d, q->cap=%2d\n", i, q_len(&q), q.tail, q.head, q.cap);
    }

    // 弹出
    for (i = 0; i < 44; i++) {
        q_pop(&q);
    printf("i = %2d, q_len = %2d, q->tail=%2d, q->head=%2d, q->cap=%2d\n", i, q_len(&q), q.tail, q.head, q.cap);
    }

    q_release(&q);
}
