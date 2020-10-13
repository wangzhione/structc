#pragma once

#include "struct.h"

struct array {
    unsigned size;      // 元素大小
    unsigned cap;       // 数组容量
    unsigned len;       // 数组长度
    void *  data;       // 数组存储
};

// array_t - 动态数组类型
typedef struct array * array_t;

//
// array_declare - 栈上创建动态数组对象
// array_free    - 销毁栈上动态数组对象
// var           : 创建动态数组对象名字
// ARRAY_UINT    - 数组初始化默认大小
#define ARRAY_UINT      (1u<<5)
#define array_declare(type, var)                \
struct array var[1] = { {                       \
    .size = sizeof(type),                       \
    .cap = ARRAY_UINT,                          \
    .data = malloc(sizeof(type) * ARRAY_UINT)   \
} }

inline void array_init(array_t a, unsigned size) {
    assert(a && size > 0);
    a->size = size;
    // set default cap size
    a->cap = ARRAY_UINT;
    a->len = 0;
    a->data = malloc(size * ARRAY_UINT);
}

inline void array_free(array_t a) {
    free(a->data);
}

//
// array_create - 返回创建动态数组对象
// size     : 元素大小
// return   : 返回创建的动态数组对象
//
inline array_t array_create(unsigned size) {
    struct array * a = malloc(sizeof(struct array));
    array_init(a, size);
    return a;
}

//
// array_delete - 销毁动态数组对象
// a        : 动态数组对象 
// return   : void
//
inline void array_delete(array_t a) {
    if (a) {
        array_free(a);
        free(a);
    }
}

//
// array_push - 数组中插入一个数据
// a        : 动态数组对象
// return   : void * 压入数据首地址
//
inline void * array_push(array_t a) {
    if (a->len >= a->cap) {
        /* the array is full; allocate new array */
        a->cap <<= 1;
        a->data = realloc(a->data, a->cap * a->size);
    }

    return (char *)a->data + a->size * a->len++;
}

//
// array_pop - 数组中弹出一个数据
// a        : 动态数组对象
// return   : void * 返回数据首地址
//
inline void * array_pop(array_t a) {
    assert(a && a->len > 0);
    --a->len;
    return (char *)a->data + a->size * a->len;
}

//
// array_top - 得到动态数组顶元素
// a        : 动态数组
// return   : 得到返回动态数组顶部元素
//
inline void * array_top(array_t a) {
    assert(a && a->len > 0);
    return (char *)a->data + a->size * (a->len - 1);
}

//
// array_get - 索引映射数组元素
// a        : 动态数组
// idx      : 索引位置
// return   : NULL is not found
//
inline void * array_get(array_t a, unsigned idx) {
    assert(a && idx < a->len);
    return (char *)a->data + a->size * idx;
}

//
// array_idx - 通过结点返回索引
// a        : 动态数组
// elem     : 查询元素
// return   : 索引
//
inline unsigned array_idx(array_t a, void * elem) {
    unsigned off = (unsigned)((char *)elem - (char *)a->data);
    assert(a && elem >= a->data && off % a->size == 0);
    return off / a->size;
}

//
// array_swap - 动态数组交换
// a        : 动态数组
// b        : 动态数组
// return   : void
//
inline void array_swap(array_t a, array_t b) {
    struct array t = *a;
    *a = *b; 
    *b = t;
}

//
// array_sort - 动态数组排序
// a        : 动态数组
// fcmp     : 数组元素的比较函数
// return   : void
//
inline void array_sort(array_t a, cmp_f fcmp) {
    assert(a && a->len && fcmp);
    qsort(a->data, a->len, a->size, 
         (int (*)(const void *, const void *))fcmp);
}

//
// array_each - 动态数组遍历
// a        : 动态数组
// func     : 遍历行为
// return   : >= 0 表示成功, < 0 表示失败
//
extern int array_each(array_t a, each_f func, void * arg);
