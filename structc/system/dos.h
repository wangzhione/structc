#ifndef _H_DOS
#define _H_DOS

/*
    平台相关提供的简单检测宏 
 */

//
// 平台检测 : NOWOS == LINUS | WINDS | MACOS
// 但是我们目前只支持, 最新的 WINDS CL 和 LINUX GCC 环境
//
#define LINUS   (1)
#define WINDS   (2)
#define MACOS   (3)

#if defined(_MSC_VER)
#   define NOWOS   WINDS
#elif defined(__APPLE__)
#   define NOWOS   MACOS
#elif defined(__GNUC__)
#   define NOWOS   LINUS
#else
#   error ERR (￣︶￣) OS
#endif

//
// 测试环境检测 : DEBUG or RELEASE
// ISDEBUG EXISTS 表示是测试环境, 否则是 RELEASE 发布环境
//
#if defined(_DEBUG) || defined(DEBUG)
#   define ISDEBUG
#endif

//
// CPU 检测 : x64 or x86
// ISX64() == true 表示 x64 否则 x86
//
inline _Bool ISX64() {
    return 8 == sizeof ((void *)0);
}

// 
// 大小端检测 : ISBIG() == true 表示大端
//
inline _Bool ISBIG() {
    union { char c[sizeof(unsigned)]; unsigned u; } e = { { 's', '?', '?', 'b' } };
    return (char)e.u == 'b';
}

#endif//_H_DOS
