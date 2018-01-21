#ifndef _H_ARRAY
#define _H_ARRAY

#include <struct.h>

struct array {
    unsigned alloc;     // 数组中元素大小
    unsigned size;      // 当前数组的容量
    unsigned len;       // 当前数组的长度
    void *   as;        // 数组存储首地址
};

// 定义可变数组类型
typedef struct array * array_t;

// array_init - array 初始化 size
static inline void array_init(array_t a, unsigned size) {
    a->as = realloc(a->as, size * a->alloc);
    if (a->len > size)
        a->len = size;
    a->size = size;
}

//
// ARRAY_CREATE - 栈上面创建可变数组对象
// ARRAY_DELETE - 销毁栈上面可变数组对象
// var      : 创建可变数组对象名字
// alloc    : 可变数组对象大小
// size     : 可变数组的初始化容量
//
#define ARRAY_CREATE(var, alloc, size)  \
struct array var[1] = { { alloc } };    \
array_init(var, size)

#define ARRAY_DELETE(var)               \
free(var->as)                           \

//
// array_create - 返回创建可变数组对象
// alloc    : 可变数组对象大小
// size     : 可变数组的初始化容量
// return   : 返回创建的可变数组对象
//
extern array_t array_create(unsigned alloc, unsigned size);

//
// array_delete - 销毁可变数组对象
// a        : 可变数组对象 
// return   : void
//
extern void array_delete(array_t a);

#endif//_H_ARRAY
