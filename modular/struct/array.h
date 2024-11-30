#pragma once

#include "struct.h"

struct array {
    unsigned size;      // 元素大小
    unsigned cap;       // 数组容量
    unsigned len;       // 数组长度
    void *  data;       // 数组存储
};


// init array success return true, size is 元素大小
extern bool array_init(struct array * a, unsigned size);

// push array element success return not nullptr
extern void * array_push(struct array * a);

inline void array_release(struct array * a) {
    free(a->data);
}

//
// array_pop - 数组中弹出一个数据
// a        : 动态数组对象
// return   : void * 返回数据首地址
//
inline void * array_pop(struct array * a) {
    assert(a && a->len > 0);
    --a->len;
    return (char *)a->data + a->size * a->len;
}

//
// struct array *op - 得到动态数组顶元素
// a        : 动态数组
// return   : 得到返回动态数组顶部元素
//
inline void * array_top(struct array * a) {
    assert(a && a->len > 0);
    return (char *)a->data + a->size * (a->len - 1);
}

//
// array_get - 索引映射数组元素
// a        : 动态数组
// idx      : 索引位置
// return   : NULL is not found
//
inline void * array_get(struct array * a, unsigned idx) {
    assert(a && idx < a->len);
    return (char *)a->data + a->size * idx;
}

//
// array_idx - 通过结点返回索引
// a        : 动态数组
// elem     : 查询元素
// return   : 索引
//
inline unsigned array_idx(struct array * a, void * elem) {
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
inline void array_swap(struct array * a, struct array * b) {
    struct array t = *a; *a = *b; *b = t;
}

//
// array_sort - 动态数组排序
// a        : 动态数组
// fcmp     : 数组元素的比较函数, 类似 __compar_fn_t 类型
//            typedef int (*__compar_fn_t) (const void * left, const void * right);
//            left > right return > 0 ; left = right return 0; left < right return < 0
// return   : void
//
inline void array_sort(struct array * a, cmp_f fcmp) {
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
extern int array_each(struct array * a, each_f func, void * arg);
