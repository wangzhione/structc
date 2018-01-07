#ifndef _H_STRUCT
#define _H_STRUCT

#include <strext.h>

//
// icmp_f - 比较行为的类型
//  : int add_cmp(const void * now, const void * node)
//
typedef int (* icmp_f)();

//
// vnew_f - 根据规则构建对象
//  : void * rb_new(void * node)
//
typedef void * (* vnew_f)();

//
// node_f - 销毁当前对象节点
//  : void list_die(void * node); 
//
typedef void (* node_f)(void * node);

//
// each_f - each 循环操作, arg 外部参数, node 是内部结点
//  : int dict_echo(struct dict * node, void * arg) { return 0; }
//
typedef int (* each_f)(void * node, void * arg);

//
// start_f - pthread create func
//  : int * run(int * arg)
//
typedef void * (* start_f)(void * arg);

#endif//_H_STRUCT
