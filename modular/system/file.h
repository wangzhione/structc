#pragma once

#include "struct.h"
#include "strext.h"

//
// file_f - 文件更新行为
//
typedef void (* file_f)(FILE * c, void * arg);

//
// file_set - 文件注册更新行为
// path     : 文件路径
// func     : NULL 标记清除, 正常 update -> func(path -> FILE, arg)
// arg      : func 额外参数
// return   : void
//
extern void file_set(const char * path, file_f func, void * arg);

//
// file_update - 配置文件刷新操作
// return   : void
//
extern void file_update(void);
