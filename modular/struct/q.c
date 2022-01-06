﻿#include "q.h"

//
// q_pop - 队列中弹出消息数据
// q      : 队列对象
// return : 若队列 empty, 返回 NULL
//
void * 
q_pop(q_t q) {
    void * m = NULL;
    if (q_exist(q)) {
        m = q->data[q->head];
        if (q->tail != q->head)
            q->head = (q->head + 1) & (q->cap - 1);
        else {
            q->head =  0; // empty 情况, 重置 tail 和 head
            q->tail = -1;
        }
    }
    return m;
}

// q_expand - expand memory by twice
static void q_expand(q_t q) {
    int i, cap = q->cap << 1;
    void ** p = malloc(sizeof(void *) * cap);
    for (i = 0; i < q->cap; ++i)
        p[i] = q->data[(q->head + i) & (q->cap - 1)];
    free(q->data);

    // 重新构造内存关系
    q->tail = q->cap;
    q->cap = cap;
    q->data = p;
    q->head = 0;
}

//
// q_push - 队列中压入数据
// q      : 队列对象
// m      : 压入消息
// return : void
// 
void 
q_push(q_t q, void * m) {
    int tail = (q->tail + 1) & (q->cap - 1);
    // 队列 full 直接扩容
    if (tail == q->head && q->tail >= 0)
        q_expand(q);
    else
        q->tail = tail;
    q->data[q->tail] = m;
}

//
// q_delete - 队列删除
// q        : 队列对象
// fdie     : node_f push 结点删除行为
// return   : void
//
void 
q_delete(q_t q, node_f fdie) {
    // 销毁所有对象
    if (fdie && q_exist(q)) {
        for (;;) {
            fdie(q->data[q->head]);
            if (q->head == q->tail)
                break;
            q->head = (q->head + 1) & (q->cap - 1);
        }
    }

    q_free(q);
}
