#include "array.h"

//
// array_create - 返回创建动态数组对象
// size     : 元素大小
// return   : 返回创建的动态数组对象
//
inline array_t 
array_create(unsigned size) {
    struct array * a = malloc(sizeof(struct array));
    assert(NULL != a && size > 0);
    // set default cap size
    a->cap = ARRAY_UINT;
    a->data = malloc(size * a->cap);
    a->size = size;
    a->len = 0;
    return a;
}

//
// array_delete - 销毁动态数组对象
// a        : 动态数组对象 
// return   : void
//
inline void 
array_delete(array_t a) {
    if (a) {
        free(a->data);
        free(a);
    }
}

//
// array_push - 数组中插入一个数据
// a        : 动态数组对象
// return   : void * 压入数据首地址
//
inline void * 
array_push(array_t a) {
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
inline void * 
array_pop(array_t a) {
    assert(NULL != a && a->len > 0);
    --a->len;
    return (char *)a->data + a->size * a->len;
}

//
// array_top - 得到动态数组顶元素
// a        : 动态数组
// return   : 得到返回动态数组顶部元素
//
inline void * 
array_top(array_t a) {
    assert(NULL != a && a->len > 0);
    return (char *)a->data + a->size * (a->len - 1);
}

//
// array_get - 索引映射数组元素
// a        : 动态数组
// idx      : 索引位置
// return   : NULL is not found
//
inline void * 
array_get(array_t a, unsigned idx) {
    assert(NULL != a && idx < a->len);
    return (char *)a->data + a->size * idx;
}

//
// array_idx - 通过节点返回索引
// a        : 动态数组
// elem     : 查询元素
// return   : 索引
//
inline unsigned 
array_idx(array_t a, void * elem) {
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
inline void 
array_swap(array_t a, array_t b) {
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
inline void 
array_sort(array_t a, cmp_f fcmp) {
    assert(NULL != a && a->len && fcmp != NULL);
    qsort(a->data, a->len, a->size, 
         (int (*)(const void *, const void *))fcmp);
}

//
// array_each - 动态数组遍历
// a        : 动态数组
// func     : 遍历行为
// return   : >= 0 表示成功, < 0 表示失败
//
int 
array_each(array_t a, each_f func, void * arg) {
    assert(NULL != a && func != NULL);
    char * s = a->data;
    char * e = s + a->size * a->len;
    while (s < e) {
        int ret = func(s, arg);
        if (ret < 0)
            return ret;
        s += a->size;
    }

    return 0;
}
