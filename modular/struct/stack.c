#include "stack.h"

// struct stack 对象栈
// stack empty <=> tail = -1 
// stack full  <=> tail == cap
// 

#define INT_STACK   (1 << 8)

bool 
stack_init(struct stack * s) {
    assert(s != nullptr);

    void * data = malloc(sizeof(void *) * INT_STACK);
    if (data == nullptr) {
        RETURN(false, "malloc panic INT_STACK = %d", INT_STACK);
    }

    s->tail = -1;
    s->cap = INT_STACK;
    s->data = data;
    return true;
}

//
// stack_push - 压入元素到对象栈栈顶
// s        : stack 对象栈
// m        : 待压入的对象
// return   : void
// 
bool 
stack_push(struct stack * s, void * m) {
    assert(s != nullptr);
    
    if (s->cap <= s->tail || s->cap <= 0) {
        int cap = s->cap <= 0 ? INT_STACK : s->cap * 2;
        void * data = realloc(s->data, sizeof(void *) * cap);
        if (data == nullptr) {
            RETURN(false, "realloc panic s = %p, cap = %d, tail = %d", s, cap, s->tail);
        }
        s->cap = cap;
        s->data = data;
    }

    s->data[++s->tail] = m;
    return true;
}
