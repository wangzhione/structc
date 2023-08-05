#include "array.h"

//
// array_each - 动态数组遍历
// a        : 动态数组
// func     : 遍历行为
// return   : >= 0 表示成功, < 0 表示失败
//
int 
array_each(struct array * a, each_f func, void * arg) {
    assert(a != NULL && func != NULL);

    char * s = a->data, * e = s + a->size * a->len;
    while (s < e) {
        int ret = func(s, arg);
        if (ret < 0)
            return ret;
        s += a->size;
    }

    return 0;
}

// ARRAY_UINT    - 数组初始化默认大小
#define ARRAY_UINT      (1u<<5)

// init array success return true
bool 
array_init(struct array * a, unsigned size) {
    assert(a != NULL && size > 0);

    void * data = malloc(size * ARRAY_UINT);
    if (data == NULL) {
        RETURN(false, "malloc panic ARRAY_UINT = %u, a = %p, size = %u", ARRAY_UINT, a, size);
    }
    a->size = size;
    a->cap = ARRAY_UINT;
    a->data = data;
    a->len = 0;
    return true;
}

//
// array_push - 数组中插入一个数据
// a        : 动态数组对象
// return   : void * 压入数据首地址, NULL 表示 PUSH 失敗
//
inline void * 
array_push(struct array * a) {
    assert(a != NULL);

    if (a->len >= a->cap) {
        /* the array is full; allocate new array */
        unsigned cap = a->cap <= 0 ? ARRAY_UINT : a->cap * 2;
        void * data = realloc(a->data, a->cap * a->size);
        if (data == NULL) {
            RETNUL("realloc panic data = %p, cap = %u, size = %u", a->data, cap, a->size);
        }
        a->cap = cap;
        a->data = data;
    }

    return (char *)a->data + a->size * a->len++;
}