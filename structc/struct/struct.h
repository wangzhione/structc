#ifndef _H_STRUCT
#define _H_STRUCT

#include <stdext.h>

//
// cmp_f - 比较行为的类型
//  : int add_cmp(const void * now, const void * node)
//  : (cmp_f)add_cmp
//
typedef int (* cmp_f)();

//
// new_f - 根据规则构建对象
//  : void * rb_new(void * node)
//  : (new_f)rb_new
//
typedef void * (* new_f)();

//
// die_f - 销毁当前对象节点
//  : void list_die(void * node);
//  : (die_f)list_die  
//
typedef void (* die_f)();

#endif//_H_STRUCT