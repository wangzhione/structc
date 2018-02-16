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
// CERR - 打印错误信息, 添加了一些特有的宏操作
// CERR_EXIT - 打印错误信息, 并直接exit
// CERR_IF - 条件判断异常退出的辅助宏
//
#define CERR(fmt, ...)                                                  \
fprintf(stderr, "[%s:%s:%d][%d:%s]" fmt "\r\n",                         \
    __FILE__, __func__, __LINE__, errno, strerror(errno), ##__VA_ARGS__)

#define CERR_EXIT(fmt, ...)     \
do {                            \
    CERR(fmt, ##__VA_ARGS__);   \
    exit(EXIT_FAILURE);         \
} while(0)


#define CERR_IF(cond)   \
if ((cond))             \
    CERR_EXIT(#cond)

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

 * DEBUG_CODE({
 *	puts("debug start...");
 * });
 */
#ifndef DEBUG_CODE
# ifdef _DEBUG
#	define DEBUG_CODE(code) \
        do code while(0)
# else
#	define DEBUG_CODE(code) 
# endif	//	! _DEBUG
#endif	//	! DEBUG_CODE

#endif//_H_ASSEXT
