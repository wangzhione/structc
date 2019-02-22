#ifndef _HEAD_H
#define _HEAD_H

#include "log.h"
#include "conf.h"
#include "file.h"
#include "check.h"
#include "thread.h"

#ifdef _MSC_VER

#include <conio.h>

inline void cls(void) {
    system("cls");
}

#endif

#ifdef __GNUC__

#include <unistd.h>
#include <termios.h>

// cls - 屏幕清除, 依赖系统脚本
inline void cls(void) {
    printf("\ec");
}

// getch - 立即得到用户输入的一个字符
inline static int getch(void) {
    struct termios now, old;
    if (tcgetattr(0, &old)) // 得到当前终端(0表示标准输入)的设置
        return EOF;
    now = old;

    // 设置终端为 Raw 原始模式，该模式下输入数据全以字节单位被处理
    cfmakeraw(&now);
    if (tcsetattr(0, TCSANOW, &now)) // 设置上更改之后的设置
        return EOF;

    int c = getchar();

    if (tcsetattr(0, TCSANOW, &old)) // 设置还原成老的模式
        return EOF;
    return c;
}

#endif

// spause - 程序结束后等待
inline static void spause(void) {
    rewind(stdin);
    fflush(stderr); fflush(stdout);
    printf("Press any key to continue . . .");
    getch();
}

//
// STR - 添加双引号的宏 
// v        : 待添加双引号的量
//
#define STR(v) S_R(v)
#define S_R(v) #v

//
// LEN - 获取数组长度
// a        : 数组名
//
#define LEN(a) sizeof(a)/sizeof(*(a))

//
// EXTERN_RUN - 简单声明, 并立即使用的宏
// frun     : 需要执行的函数名称
// ...      : 可变参数, 保留
//
#define EXTERN_RUN(frun, ...)                                \
do {                                                         \
    extern void frun();                                      \
    frun (__VA_ARGS__);                                      \
} while(0)

//
// TEST_RUN - 测试代码块, 并输出运行时间
// code     : { ... } 包裹的代码块
//
#define TEST_RUN(code)                                      \
do {                                                        \
    clock_t $s = clock();                                   \
    code                                                    \
    double $e = (double)clock();                            \
    printf("code run time:%lfs\n", ($e-$s)/CLOCKS_PER_SEC); \
} while (0)

#endif//_HEAD_H
