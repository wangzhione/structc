#pragma once

#include "struct.h"

// struct stack 对象栈
// stack empty <=> tail = -1 
// stack full  <=> tail == cap
// 
struct stack {
    int      tail;  // 尾结点
    int       cap;  // 栈容量
    void **  data;  // 栈实体
};

extern bool stack_init(struct stack * s);
extern bool stack_push(struct stack * s, void * m);

inline void stack_release(struct stack * s) {
    free(s->data);
}

//
// stack_delete - 删除 stack 对象栈
// s        : stack 对象栈
// fdie     : node_f push 结点删除行为
// return   : void
//
inline void stack_delete(struct stack * s, node_f fdie) {
    if (s != nullptr) {
        if (fdie != nullptr) {
            while (s->tail >= 0)
                fdie(s->data[s->tail--]);
        }
        // stack_create 和 free 对应
        free(s->data);
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

inline bool stack_exist(struct stack * s) {
    return s->tail >= 0;
}

//
// stack_top - 获取 stack 栈顶对象
// s        : stack 对象栈
// return   : 栈顶对象
//
inline void * stack_top(struct stack * s) {
    assert(s != nullptr && s->tail >= 0);
    return s->data[s->tail];
}

inline int stack_len(struct stack * s) {
    return s->tail+1;
}

//
// stack_popped - 弹出栈顶元素
// s        : stack 对象栈
// return   : void
//
inline void stack_popped(struct stack * s) {
    assert(s != nullptr && s->tail >= 0);
    s->tail--;
}

//
// stack_pop - 弹出并返回栈顶元素
// s        : stack 对象栈
// return   : 弹出的栈顶对象
//
inline void * stack_pop(struct stack * s) {
    assert(s != nullptr && s->tail >= 0);
    return s->data[s->tail--];
}
