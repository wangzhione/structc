#ifndef _STACK_H
#define _STACK_H

#include "struct.h"

#define INT_STACK   (1 << 8)

// 
// struct stack 对象栈
// stack empty <=> tail = -1 
// stack full  <=> tail == cap
//
struct stack {
    int      tail;  // 尾结点
    int       cap;  // 栈容量
    void **  data;  // 栈实体
};

//
// stack_init - 初始化 stack 对象栈
// return   : void
//
inline void stack_init(struct stack * s) {
    assert(s && INT_STACK > 0);
    s->tail = -1;
    s->cap = INT_STACK;
    s->data = malloc(sizeof(void *) * INT_STACK);
}

//
// stack_create - 创建 stack 对象栈
// var      : stack 对象栈
// 
//
#define stack_create(var)                           \
struct stack var[1] = {{                            \
    .tail = -1,                                     \
     .cap = INT_STACK,                              \
    .data = malloc(sizeof(void *) * INT_STACK),     \
}}

//
// stack_delete - 删除 stack 对象栈
// s        : stack 对象栈
// fdie     : node_f push 结点删除行为
// return   : void
//
inline void stack_delete(struct stack * s, node_f fdie) {
    if (fdie) {
        while (s->tail >= 0)
            fdie(s->data[s->tail--]);
    }
    free(s->data);
}

//
// stack_top - 获取 stack 栈顶对象
// s        : stack 对象栈
// return   : 栈顶对象
//
inline void * stack_top(struct stack * s) {
    return s->tail >= 0 ? s->data[s->tail] : NULL;
}

//
// stack_pop - 弹出栈顶元素
// s        : stack 对象栈
// return   : 栈顶弹出后的对象
//
inline void * stack_pop(struct stack * s) {
    return s->tail >= 0 ? s->data[s->tail--] : NULL;
}

//
// stack_push - 压入元素到对象栈栈顶
// s        : stack 对象栈
// m        : 待压入的对象
// return   : void
// 
inline void stack_push(struct stack * s, void * m) {
    if (s->tail >= s->cap) {
        s->cap <<= 1;
        s->data = realloc(s->data, sizeof(void *) * s->cap);
    }
    s->data[++s->tail] = m;
}

#endif//_STACK_H
