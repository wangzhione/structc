#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef _MSC_VER

//
// CPU 检测 x64 or x86
// ISX64 defined 表示 x64 否则 x86
//
#if defined _M_ARM64 || defined_M_X64
#  define ISX64
#endif

//
// _M_PPC 为 PowerPC 平台定义, 现在已不支持
//
#if defined _M_PPC
#  define ISBIG
#endif

#define __attribute__(x)        /* 开眼不知何处, */
#define _Noreturn               /* 但听满耳松风。*/

#define PACKED(declare)         \
__pragma (pack(push, 1))        \
declare                         \
__pragma (pack(pop))

#else

#define PACKED(declare)         \
declare                         \
__attribute__((__packed__))

#if defined __x86_64__
#  define ISX64
#endif

//
// 大小端检测 : ISBENIAN defined 表示大端
//
#if defined __BIG_ENDIAN__ || defined __BIG_ENDIAN_BITFIELD
#  define ISBIG
#endif

#endif

// small - 转本地字节序(小端)
inline uint32_t small(uint32_t x) {
#  ifdef ISBIG
    uint8_t t;
    uint8_t * p = (uint8_t *)&x;

    t = p[0]; p[0] = p[3]; p[3] = t;
    t = p[1]; p[1] = p[2]; p[2] = t;
#  endif
    return x;
}
