#ifndef _DICT_H
#define _DICT_H

#include "struct.h"
#include "strext.h"

//
// dict_t - C 字符串为 key 的字典结构
//
typedef struct dict * dict_t;

//
// dict_delete - 字典删除
// d        : dict_create 创建的字典对象
// return   : void 
//
extern void dict_delete(dict_t d);

//
// dict_create - 字典创建
// fdie     : node_f 销毁行为
// return   : dict_t
//
extern dict_t dict_create(node_f fdie);

//
// dict_get - 获取字典中对映的 v
// d        : dict_create 创建的字典对象
// k        : 查找的 key 
// return   : 查找的 v, NULL 表示没有
//
extern void * dict_get(dict_t d, const char * k);

//
// dict_set - 设置一个 <k, v> 结构
// d        : dict_create 创建的字典对象
// k        : 插入的 key
// v        : 插入数据的值, NULL 会销毁 k
// return   : void
//
extern void dict_set(dict_t d, const char * k, void * v);

#endif//_DICT_H
