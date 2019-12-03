﻿#ifndef __STRUCTC_STRUCT_HEAP_H
#define __STRUCTC_STRUCT_HEAP_H

#include "struct.h"

//
// head_t 堆的数据结构
//
typedef struct heap * heap_t;

//
// heap_create - 创建符合规则的堆
// fcmp     : 比较行为, 规则 fcmp() <= 0
// return   : 返回创建好的堆对象
//
extern heap_t heap_create(cmp_f fcmp);

//
// heap_delete - 销毁堆
// h        : 堆对象
// fdie     : 销毁行为, 默认 NULL
// return   : void
//
extern void heap_delete(heap_t h, node_f fdie);

//
// heap_insert - 堆插入数据
// h        : 堆对象
// node     : 操作对象
// return   : void
//
extern void heap_insert(heap_t h, void * node);

//
// heap_remove - 堆删除数据
// h        : 堆对象
// arg      : 操作参数
// fcmp     : 比较行为, 规则 fcmp() == 0
// return   : 找到的堆结点
//
extern void * heap_remove(heap_t h, void * arg, cmp_f fcmp);

//
// heap_top - 查看堆顶结点数据
// h        : 堆对象
// return   : 堆顶结点
//
extern void * heap_top(heap_t h);

//
// heap_top - 摘掉堆顶结点数据
// h        : 堆对象
// return   : 返回堆顶结点
//
extern void * heap_pop(heap_t h);

#endif//__STRUCTC_STRUCT_HEAP_H
