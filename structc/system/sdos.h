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

#endif//_H_SDOS
