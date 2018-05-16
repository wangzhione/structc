#ifndef _H_Q
#define _H_Q

//
// pop empty  <=> tail == -1 ( head == 0 )
// push full  <=> head == tail + 1
//
typedef struct q {
    int head;           // 头结点
    int tail;           // 尾结点
    int size;           // 队列大小
    void ** queue;      // 队列实体
} q_t[1];

#endif//_H_Q
