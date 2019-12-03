#ifndef __STRUCTC_SYSTEM_SYSTEM_H
#define __STRUCTC_SYSTEM_SYSTEM_H

#include <stdint.h>
#include <stddef.h>

#ifdef _MSC_VER
//
// CPU 检测 x64 or x86
// ISX64 defined 表示 x64 否则 x86
//
#  if defined(_M_ARM64) || defined(_M_X64)
#    define ISX64
#  endif
//
// _M_PPC 为 PowerPC 平台定义, 现在已不支持
//
#  if defined(_M_PPC)
#    define ISBIG
#  endif
#else
#  if defined(__x86_64__)
#    define ISX64
#  endif
//
// 大小端检测 : ISBENIAN defined 表示大端
//
#  if defined(__BIG_ENDIAN__) || defined(__BIG_ENDIAN_BITFIELD)
#    define ISBIG
#  endif
#endif

// small - 转本地字节序(小端)
inline uint32_t small(uint32_t x) {
#  ifndef ISBIG
    return x;
#  else
    uint8_t t;
    union { uint32_t i; uint8_t s[sizeof(uint32_t)]; }u = { x };
    t = u.s[0]; u.s[0] = u.s[sizeof(u)-1]; u.s[sizeof(u)-1] = t;
    t = u.s[1]; u.s[1] = u.s[sizeof(u)-2]; u.s[sizeof(u)-2] = t;
    return u.i;
#  endif
}

#endif//__STRUCTC_SYSTEM_SYSTEM_H
