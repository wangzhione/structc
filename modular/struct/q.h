#pragma once

#include "struct.h"

 // q simple ringlike queue
// pop empty <=> tail == -1 ( head == 0 )
// push full <=> head == (tail + 1) % cap && tail >= 0
//
typedef struct q {
    int     head;       // 头结点索引
    int     tail;       // 尾结点索引
    int      cap;       // 队列容量
    void ** data;       // 队列实体
} q_t[1];

// Q_INT  - 队列初始大小, 必须是 2 的幂
#ifndef Q_INT
#define Q_INT     (1<< 6)
#endif

//
// q_init - 初始化
// q      : 队列对象
// return : void
inline void q_init(q_t q) {
    q->data = malloc(sizeof(void *)*Q_INT);
    q->cap = Q_INT;
    q->tail = -1; q->head =  0;
}

inline void q_free(q_t q) {
    free(q->data);
}

inline bool q_empty(q_t q) {
    return q->tail < 0;
}

inline bool q_exist(q_t q) {
    return q->tail >=0;
}

inline int q_len(q_t q) {
    // return q->tail < 0 ? 0 :( 
    //  q->tail < q->head ? q->cap+q->tail-q->head+1 : q->tail-q->head+1
    // );
    // q->tail >= 0       ? 1 : 0
    // q->tail < q-> head ? 1 : 0
    return (q->tail >= 0)*((q->tail < q->head)*q->cap + q->tail-q->head+1);
}

//
// q_swap - q swap
// r      : q one
// w      : q two
// return : void
//
inline void q_swap(q_t r, q_t w) {
    q_t q; q[0] = r[0]; r[0] = w[0]; w[0] = q[0];
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
// fdie     : node_f push 结点删除行为
// return   : void
//
extern void q_delete(q_t q, node_f fdie);
