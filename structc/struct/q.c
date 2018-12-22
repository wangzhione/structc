#include "q.h"

//
// q_delete - 队列销毁
// q        : 队列对象
// fdie     : push 节点删除行为
// return   : void
//
void 
q_delete(q_t q, node_f fdie) {
    // 销毁所有对象
    if (q->tail >= 0 && fdie) {
        for (;;) {
            fdie(q->queue[q->head]);
            if (q->head == q->tail)
                break;
            q->head = (q->head + 1) & (q->size - 1);
        }
    }

    free(q->queue);
}

// q_expand - expand memory by twice
static void q_expand(q_t q) {
    int i, size = q->size << 1;
    void ** nq = malloc(sizeof(void *) * size);
    for (i = 0; i < q->size; ++i)
        nq[i] = q->queue[(q->head + i) & (q->size - 1)];
    free(q->queue);

    // 重新构造内存关系
    q->head = 0;
    q->tail = q->size;
    q->size = size;
    q->queue = nq;
}

//
// q_push - 队列中压入数据
// q      : 队列对象
// m      : 压入消息
// return : void
// 
void 
q_push(q_t q, void * m) {
    int tail = (q->tail + 1) & (q->size - 1);
    // 队列 full 直接扩容
    if (tail == q->head && q->tail >= 0)
        q_expand(q);
    else
        q->tail = tail;
    q->queue[q->tail] = m;
}
