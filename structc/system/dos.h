#ifndef _H_DOS
#define _H_DOS

/*
    平台相关提供的简单检测宏 
 */

//
// 编译平台检测 : winds cl -> WCL | LGCC -> linux gcc
// 目前我们只支持, 最新的 WINDS CL 和 LINUX GCC 环境
//
#if defined(__GNUC__)
#   define LGCC (1)
#elif defined(_MSC_VER)
#   define WCL  (2)
#else
#   error BUILD (￣︶￣) S
#endif

//
// 测试环境检测 : DEBUG or RELEASE
// ISDEBUG EXISTS 表示是测试环境, 否则是 RELEASE 发布环境
//
#if defined(_DEBUG) || defined(DEBUG)
#   define ISDEBUG
#endif

#if defined(WCL)

//
// CPU 检测 : x64 or x86
// ISX64 defined 表示 x64 否则 x86
//
#   if defined(_M_ARM64) || defined(_M_X64)
#       define ISX64
#   endif

#   if defined(_M_PPC)
#       define ISBENIAN
#endif

#endif

#if defined(LGCC)

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

#endif//_H_DOS
