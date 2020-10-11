#pragma once

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
// stack_free - 清除掉 stack 对象栈
// return   : void
//
inline void stack_init(struct stack * s) {
    assert(s && INT_STACK > 0);
    s->tail = -1;
    s->cap  = INT_STACK;
    s->data = malloc(sizeof(void *) * INT_STACK);
}

inline void stack_free(struct stack * s) {
    free(s->data);
}

//
// stack_delete - 删除 stack 对象栈
// s        : stack 对象栈
// fdie     : node_f push 结点删除行为
// return   : void
//
inline void stack_delete(struct stack * s, node_f fdie) {
    if (s) {
        if (fdie) {
            while (s->tail >= 0)
                fdie(s->data[s->tail--]);
        }
        stack_free(s);
    }
}

//
// stack_empty - 判断 stack 对象栈是否 empty
// s        : stack 对象栈
// return   : true 表示 empty
//
inline bool stack_empty(struct stack * s) {
    return s->tail <  0;
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
// return   : void
//
inline void stack_pop(struct stack * s) {
    if (s->tail >= 0) --s->tail;
}

//
// stack_pop_top - 弹出并返回栈顶元素
// s        : stack 对象栈
// return   : 弹出的栈顶对象
//
inline void * stack_pop_top(struct stack * s) {
    return s->tail >= 0 ? s->data[s->tail--] : NULL;
}

//
// stack_push - 压入元素到对象栈栈顶
// s        : stack 对象栈
// m        : 待压入的对象
// return   : void
// 
inline void stack_push(struct stack * s, void * m) {
    if (s->cap <= s->tail) {
        s->cap <<= 1;
        s->data = realloc(s->data, sizeof(void *) * s->cap);
    }
    s->data[++s->tail] = m;
}
