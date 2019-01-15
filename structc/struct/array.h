#ifndef _ARRAY_H
#define _ARRAY_H

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
// ARRAY_CREATE - 栈上创建动态数组对象
// ARRAY_DELETE - 销毁栈上动态数组对象
// var          : 创建动态数组对象名字
// ARRAY_UINT   - 数组初始化默认大小
#define ARRAY_UINT      (1u<<5)
#define ARRAY_CREATE(type, var)             \
struct array var[1] = { {                   \
    sizeof(type),                           \
    ARRAY_UINT,                             \
    0,                                      \
    malloc(sizeof(type) * ARRAY_UINT)       \
} }

#define ARRAY_DELETE(var)                   \
free((var)->data)

//
// array_create - 返回创建动态数组对象
// size     : 元素大小
// return   : 返回创建的动态数组对象
//
extern array_t array_create(unsigned size);

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
// array_top - 得到动态数组顶元素
// a        : 动态数组
// return   : 得到返回动态数组顶部元素
//
extern void * array_top(array_t a);

//
// array_get - 索引映射数组元素
// a        : 动态数组
// idx      : 索引位置
// return   : NULL is not found
//
extern void * array_get(array_t a, unsigned idx);

//
// array_idx - 通过节点返回索引
// a        : 动态数组
// elem     : 查询元素
// return   : 索引
//
extern unsigned array_idx(array_t a, void * elem);

//
// array_swap - 动态数组交换
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
extern void array_sort(array_t a, cmp_f fcmp);

//
// array_each - 动态数组遍历
// a        : 动态数组
// func     : 遍历行为
// return   : >= 0 表示成功, < 0 表示失败
//
extern int array_each(array_t a, each_f func, void * arg);

#endif//_ARRAY_H
