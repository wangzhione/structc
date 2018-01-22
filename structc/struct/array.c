#include <array.h>
#include <assert.h>

#define _UINT_ARRAY     (1u<<5)      // 数组默认大小

//
// array_create - 返回创建可变数组对象
// alloc    : 可变数组对象大小
// size     : 可变数组的初始化容量
// return   : 返回创建的可变数组对象
//
inline array_t 
array_create(unsigned alloc, unsigned size) {
    struct array * a = malloc(sizeof(struct array));
    assert(NULL != a && alloc > 0);
    // 设置默认 size 大小
    size = size ? size : _UINT_ARRAY;
    a->as = malloc(alloc * size);
    a->alloc = alloc;
    a->size = size;
    a->len = 0;
    return a;
}

//
// array_delete - 销毁可变数组对象
// a        : 可变数组对象 
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
// a        : 可变数组对象
// ptr      : 待插入结点的首地址
// return   : void * 压入数据首地址
//
void * 
array_push(array_t a, void * ptr) {
    void * rst;
    assert(NULL != a && ptr != NULL);

    if (a->len >= a->size) {
        /* the array is full; allocate new array */
        a->size <<= 1;
        a->as = realloc(a->as, a->size * a->alloc);
    }

    rst = (char *)a->as + a->alloc * a->len++;
    return memcpy(rst, ptr, a->alloc);
}

//
// array_pop - 数组中弹出一个数据
// a        : 可变数组对象
// return   : void * 返回数据首地址
//
inline void * 
array_pop(array_t a) {
    if (!a || a->len <= 0)
        return NULL;
    --a->len;
    return (char *)a->as + a->alloc * a->len;
}

