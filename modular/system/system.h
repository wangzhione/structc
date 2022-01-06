#pragma once

//
// 辽阔的夜, 黝黑而深邃 ~
//
// 跨平台模拟自娱自乐了很久, 可能也有零星的收获, 那就是认怂 ~ 
//
// 
// 苏堤春晓
// 明·杨周
// 
// 柳暗花明春正好，重湖雾散分林鸟。
// 何处黄鹂破暝烟，一声啼过苏堤晓。
//

#include <stdint.h>
#include <stddef.h>

/*
project defined template

#if defined(__linux__) && defined(__GNUC__)

#elif defined(_WIN32) && defined(_MSC_VER)

#endif
*/

//
// 约定: BEST NEW VERSION 操作系统 Linux + 编译工具 GCC
//
#if defined(__linux__) && defined(__GNUC__)

# if defined(__x86_64__)
#  define ISX64
# endif

//
// 大小端检测 : ISBIG defined 表示大端
//
# if defined(__BIG_ENDIAN__) || defined(__BIG_ENDIAN_BITFIELD)
#  define ISBIG
# endif

#define PACKED(declare)             \
declare                             \
__attribute__((__packed__))

# ifndef likely
#   define likely(x)   __builtin_expect(!!(x), 1)
# endif
# ifndef unlikely
#   define unlikely(x) __builtin_expect(!!(x), 0)
# endif

//
// 约定: BEST NEW VERSION 操作系统 window + vs
//
#elif defined(_WIN32) && defined(_MSC_VER)

//
// CPU 检测 x64 or x86
// ISX64 defined 表示 x64 否则 x86
//
# if defined _M_ARM64 || defined_M_X64
#  define ISX64
# endif

//
// _M_PPC 为 PowerPC 平台定义, 现在已不支持
//
# if defined _M_PPC
#  define ISBIG
# endif

#define PACKED(declare)             \
__pragma (pack(push, 1))            \
declare                             \
__pragma (pack(pop))

#define __attribute__(x)            /* 开眼不知何处, */
                                    /* 但听满耳松风。*/
#define likely(x)    (x)
#define unlikely(x)  (x)

#endif
