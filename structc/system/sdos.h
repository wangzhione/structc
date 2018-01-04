#ifndef _H_SDOS
#define _H_SDOS

//
// 平台相关提供的简单检测宏 :)
//
// 莫名的伤感起来 ~  
//   __GNUC__ -> Linux GCC
//   _MSC_VER -> Winds CL
//
#if !defined(__GNUC__) && !defined(_MSC_VER)
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

#endif//_H_SDOS
