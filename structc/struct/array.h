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

#define UINT_AINIT      (1u<<5)         // 数组初始化默认大小

//
// ARRAY_CREATE - 栈上面创建可变数组对象
// ARRAY_DELETE - 销毁栈上面可变数组对象
// var      : 创建可变数组对象名字
//
#define ARRAY_CREATE(type, var)         \
struct array var[1] = { {               \
    sizeof(type),                       \
    UINT_AINIT,                         \
    0,                                  \
    malloc(sizeof(type) * UINT_AINIT)   \
} }

#define ARRAY_DELETE(var)               \
free(var->as)                           \

//
// array_create - 返回创建可变数组对象
// alloc    : 可变数组对象大小
// return   : 返回创建的可变数组对象
//
extern array_t array_create(unsigned alloc);

//
// array_delete - 销毁可变数组对象
// a        : 可变数组对象 
// return   : void
//
extern void array_delete(array_t a);

//
// array_push - 数组中插入一个数据
// a        : 可变数组对象
// return   : void * 压入数据首地址
//
extern void * array_push(array_t a);

//
// array_pop - 数组中弹出一个数据
// a        : 可变数组对象
// return   : void * 返回数据首地址
//
extern void * array_pop(array_t a);

#endif//_H_ARRAY
