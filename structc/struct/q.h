#ifndef _Q_H
#define _Q_H

#include "struct.h"

//
// pop empty  <=> tail == -1 ( head == 0 )
// push full  <=> head == (tail + 1) % size
//
typedef struct q {
    void ** queue;      // 队列实体
    int size;           // 队列大小
    int head;           // 头节点
    int tail;           // 尾节点
} q_t[1];

//
// q_init - 初始化
// q      : 队列对象
// return : void
// Q_INT  - 队列初始大小, 必须是 2 的幂
#define Q_INT     (1<< 6)
inline void q_init(q_t q) {
    q->queue = malloc(sizeof(void *) * Q_INT);
    q->size = Q_INT;
    q->head = 0;
    q->tail = -1;
}

//
// q_swap - q swap
// r      : q one
// w      : q two
// return : void
//
inline void q_swap(q_t r, q_t w) {
    q_t q;
    q[0] = r[0];
    r[0] = w[0];
    w[0] = q[0];
}

//
// q_pop - 队列中弹出消息数据
// q      : 队列对象
// return : 若队列 empty, 返回 NULL
//
extern void * q_pop(q_t q);

//
// q_push - 队列中压入数据
// q      : 队列对象
// m      : 压入消息
// return : void
// 
extern void q_push(q_t q, void * m);

//
// q_delete - 队列删除
// q        : 队列对象
// fdie     : node_f push 节点删除行为
// return   : void
//
extern void q_delete(q_t q, node_f fdie);

#endif//_Q_H
