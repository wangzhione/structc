#include <array.h>
#include <assert.h>

//
// array_create - 返回创建动态数组对象
// alloc    : 动态数组对象大小
// return   : 返回创建的动态数组对象
//
inline array_t 
array_create(unsigned alloc) {
    struct array * a = malloc(sizeof(struct array));
    assert(NULL != a && alloc > 0);
    // 设置默认 size 大小
    a->size = UINT_AINIT;
    a->as = malloc(alloc * a->size);
    a->alloc = alloc;
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
        free(a->as);
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
    if (a->len >= a->size) {
        /* the array is full; allocate new array */
        a->size <<= 1;
        a->as = realloc(a->as, a->size * a->alloc);
    }

    return (char *)a->as + a->alloc * a->len++;
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
    return (char *)a->as + a->alloc * a->len;
}

//
// array_top - 得到动态数组顶元素
// a        : 动态数组
// return   : 得到返回动态数组顶部元素
//
inline void * 
array_top(array_t a) {
    assert(NULL != a && a->len > 0);
    return (char *)a->as + a->alloc * (a->len - 1);
}

//
// array_get - 按照索引得到数组元素
// a        : 动态数组
// idx      : 索引位置
// return   : NULL 表示没有找见
//
inline void * 
array_get(array_t a, unsigned idx) {
    assert(NULL != a && idx < a->len);
    return (char *)a->as + a->alloc * idx;
}

//
// array_idx - 通过结点返回索引
// a        : 动态数组
// elem     : 查询元素
// return   : 索引为 (unsigned)-1 表示不存在
//
inline unsigned 
array_idx(array_t a, void * elem) {
    unsigned off;
    assert(NULL != a && elem >= a->as);
    off = (unsigned)(elem - a->as);
    assert(off % a->alloc == 0);
    return off / a->alloc;
}

//
// array_swap - 动态数组二者交换
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
array_sort(array_t a, icmp_f fcmp) {
    assert(NULL != a && a->len && fcmp != NULL);
    qsort(a->as, a->len, a->alloc, 
        (int (*)(const void *, const void *))fcmp);
}

//
// array_each - 动态数组遍历函数, 利于测试
// a        : 动态数组
// func     : 执行每个结点函数 typedef int (* each_f)(void * node, void * arg);
// return   : >= 0 表示成功, < 0 表示失败
//
int 
array_each(array_t a, each_f func, void * arg) {
    int ret;
    char * s, * e;

    assert(NULL != a && func != NULL);
    s = a->as;
    e = s + a->alloc * a->len;
    while (s < e) {
        ret = func(s, arg);
        if (ret < 0)
            return ret;
        s += a->alloc;
    }

    return 0;
}
