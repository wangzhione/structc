#ifndef _H_ASSEXT
#define _H_ASSEXT

//
// assert.h 的扩展接口
//

#include <sdos.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//
// CERR - 打印错误信息
// EXIT - 打印错误信息, 并 exit
// IF   - 条件判断异常退出的辅助宏
//
#ifndef CERR
#define CERR(fmt, ...)                                                   \
fprintf(stderr, "[%s:%s:%d][%d:%s]" fmt "\n",                            \
    __FILE__, __func__, __LINE__, errno, strerror(errno), ##__VA_ARGS__)

#endif//CERR

#define EXIT(fmt, ...)                                              \
do {                                                                     \
    CERR(fmt, ##__VA_ARGS__);                                            \
    exit(EXIT_FAILURE);                                                  \
} while(0)

#define IF(cond)                                                    \
if ((cond)) EXIT(#cond)

//
// RETURN - 打印错误信息, 并return返回指定结果
// val		: return的东西, 当需要 return void; 时候填 ',' 就过 or NIL
// fmt		: 双引号包裹的格式化字符串
// ...		: fmt中对应的参数
// return	: val
// 
#define NIL
#define RETURN(val, fmt, ...)   \
do {                            \
    CERR(fmt, ##__VA_ARGS__);   \
    return val;                 \
} while(0)

#define RETNIL(fmt, ...)        \
RETURN(NIL, fmt, ##__VA_ARGS__)

/*
 * 这里是一个 在 DEBUG 模式下的测试宏
 *

 * DCODE({
 *	  puts("debug start...");
 * });
 */
#ifndef DCODE
# ifdef _DEBUG
#	define DCODE(code) \
        do code while(0)
# else
#   define DCODE(code) 
# endif	//	! _DEBUG
#endif	//	! DCODE

#endif//_H_ASSEXT
