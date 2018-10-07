#ifndef _H_ARRAY
#define _H_ARRAY

#include "struct.h"

struct array {
    unsigned alloc;     // 数组中元素大小
    unsigned size;      // 当前数组的容量
    unsigned len;       // 当前数组的长度
    void *   as;        // 数组存储首地址
};

// 定义动态数组类型
typedef struct array * array_t;

//
// ARRAY_CREATE - 栈上面创建动态数组对象
// ARRAY_DELETE - 销毁栈上面动态数组对象
// var      : 创建动态数组对象名字
//UINT_AINIT    - 数组初始化默认大小
#define UINT_AINIT      (1u<<5)
#define ARRAY_CREATE(type, var)             \
struct array var[1] = { {                   \
    sizeof(type),                           \
    UINT_AINIT,                             \
    0,                                      \
    malloc(sizeof(type) * UINT_AINIT)       \
} }

#define ARRAY_DELETE(var)                   \
free((var)->as)

//
// array_create - 返回创建动态数组对象
// alloc    : 动态数组大小
// return   : 返回创建的动态数组对象
//
extern array_t array_create(unsigned alloc);

//
// array_delete - 销毁动态数组对象
// a        : 动态数组对象 
// return   : void
//
extern void array_delete(array_t a);

//
// array_push - 数组中插入一个数据
// a        : 动态数组
// return   : void * 压入数据首地址
//
extern void * array_push(array_t a);

//
// array_pop - 数组中弹出一个数据
// a        : 动态数组
// return   : void * 返回数据首地址
//
extern void * array_pop(array_t a);

//
// array_each - 动态数组遍历函数, 利于测试
// a        : 动态数组
// func     : 执行每个结点函数 typedef int (* each_f)(void * node, void * arg);
// return   : >= 0 表示成功, < 0 表示失败
//
int array_each(array_t a, each_f func, void * arg);

//
// array_top - 得到动态数组顶元素
// a        : 动态数组
// return   : 得到返回动态数组顶部元素
//
extern void * array_top(array_t a);

//
// array_get - 按照索引得到数组元素
// a        : 动态数组
// idx      : 索引位置
// return   : NULL 表示没有找见
//
extern void * array_get(array_t a, unsigned idx);

//
// array_idx - 通过结点返回索引
// a        : 动态数组
// elem     : 查询元素
// return   : 索引为 (unsigned)-1 表示不存在
//
extern unsigned array_idx(array_t a, void * elem);

//
// array_swap - 动态数组二者交换
// a        : 动态数组
// b        : 动态数组
// return   : void
//
extern void array_swap(array_t a, array_t b);

//
// array_sort - 动态数组排序
// a        : 动态数组
// fcmp     : 数组元素的比较函数
// return   : void
//
extern void array_sort(array_t a, icmp_f fcmp);

#endif//_H_ARRAY
