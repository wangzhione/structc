#ifndef _H_SDOS
#define _H_SDOS

// 
// :) 
// 莫名的伤感起来 ~  
// _MSC_VER -> Winds CL
// __GNUC__ -> Linux GCC
//
#if !defined(_MSC_VER) && !defined(__GNUC__)
#   error BUILD (￣︶￣) S
#endif

#if defined(_MSC_VER)

//
// CPU 检测 : x64 or x86
// ISX64 defined 表示 x64 否则 x86
//
#   if defined(_M_ARM64) || defined(_M_X64)
#       define ISX64
#   endif

#   if defined(_M_PPC)
#       define ISBENIAN
#   endif

#endif

#if defined(__GNUC__)

#   if defined(__x86_64__)
#       define ISX64
#   endif

// 
// 大小端检测 : ISBENIAN defined 表示大端
//
#   if defined(__BIG_ENDIAN__) || defined(__BIG_ENDIAN_BITFIELD)
#       define ISBENIAN
#   endif

#endif


#ifndef _H_ELAG
#define _H_ELAG

//
// enum Elag int - 函数返回值全局状态码
// >= 0 标识 Success 状态, < 0 标识 Error 状态
//
enum {
    SBase     = +0, // 基础正确类型

    EBase     = -1, // 错误基础类型

    EParam    = -2, // 输入参数错误
    EFd       = -3, // 文件打开失败
    EClose    = -4, // 文件操作关闭
    EAlloc    = -5, // 内存分配错误
    ETout     = -5, // 操作超时错误
    EParse    = -6, // 协议解析错误
};

#endif//_H_ELAG

#endif//_H_SDOS
