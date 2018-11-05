#ifndef _H_ALLOC
#define _H_ALLOC

#include <stdlib.h>
#include <string.h>

// :) 高效内存分配, 莫名伤感 ~
// _MSC_VER -> Winds CL
// __GNUC__ -> Linux GCC
//
#ifdef _MSC_VER
//
// CPU 检测 x64 or x86
// ISX64 defined 表示 x64 否则 x86
//
#   if defined(_M_ARM64) || defined(_M_X64)
#       define ISX64
#   endif
//
// _M_PPC 为 PowerPC 平台定义, 现在已不支持 so winds 默认是小端平台
//
#   if defined(_M_PPC)
#       define ISBENIAN
#   endif

#elif  __GNUC__

#   if defined(__x86_64__)
#       define ISX64
#   endif
//
// 大小端检测 : ISBENIAN defined 表示大端
//
#   if defined(__BIG_ENDIAN__) || defined(__BIG_ENDIAN_BITFIELD)
#       define ISBENIAN
#   endif

#else
#   error BUILD (￣︶￣) S
#endif

//
// enum Flag int - 函数返回值全局状态码
// >= 0 标识 Success 状态, < 0 标识 Error 状态
//
enum {
    SBase       =  +0, // 正确基础类型

    EBase       =  -1, // 错误基础类型
    EParam      =  -2, // 输入参数错误
    EFd         =  -3, // 文件打开失败
    EClose      =  -4, // 文件操作关闭
    EAccess     =  -5, // 没有操作权限
    EAlloc      =  -6, // 内存操作错误
    EParse      =  -7, // 协议解析错误
    ESmall      =  -8, // 过小基础错误
    EBig        =  -9, // 过大基础错误
    ETimeout    = -10, // 操作超时错误
};

// OFF_ALLOC - 关闭全局 free / malloc 配置
#ifndef OFF_ALLOC

#   undef  free
#   define free    free_

#   undef  strdup
#   define strdup  strdup_

#   undef  malloc
#   define malloc  malloc_
#   undef  calloc
#   define calloc  calloc_
#   undef  realloc
#   define realloc realloc_

#endif//OFF_ALLOC

//
// free_ - free 包装函数
// ptr      : 内存首地址
// return   : void
//
extern void free_(void * ptr);

//
// malloc_ - malloc 包装, 封装一些特殊业务
// size     : 分配的内存字节
// return   : 返回可使用的内存地址.
//
extern void * malloc_(size_t size);

//
// strdup_ - strdup 包装函数
// s        : '\0' 结尾 C 字符串
// return   : 拷贝后新的 C 字符串
//
extern char * strdup_(const char * s);

//
// calloc_ - calloc 包装, 封装一些特殊业务
// num      : 数量
// size     : 大小
// return   : 返回可用内存地址, 并且置0
//
extern void * calloc_(size_t num, size_t size);

//
// realloc_ - realoc 包装函数, 封装一些特殊业务
// ptr      : 内存首地址, NULL 等同于 malloc
// size     : 重新分配的内存大小
// return   : 返回重新分配好的新地址内容
//
extern void * realloc_(void * ptr, size_t size);

#endif//_H_STDEXIT
