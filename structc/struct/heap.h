#pragma once

#include "struct.h"

//
// heap_t 堆的数据结构
//
typedef struct heap * heap_t;

//
// heap_create - 构建特定规则的初始'小顶'堆
// fcmp   : 当 fcmp(起始结点, 待比较结点) <= 0 停止调整
// return : 返回创建好的堆对象
//
extern heap_t heap_create(cmp_f fcmp);

extern void heap_delete(heap_t h, node_f fide);

extern int heap_len(heap_t h);

extern void * heap_top(heap_t h);

extern bool heap_push(heap_t h, void * node);

extern void * heap_pop(heap_t h);

//
// heap_remove - 删除堆中索引 i 数据
// h      : 堆对象
// i      : 索引 i [0, heap_len())
// return : 索引为 i 的堆结点
//
extern void * heap_remove(heap_t h, int i);

extern void * heap_pop_push(heap_t h, void * node);
