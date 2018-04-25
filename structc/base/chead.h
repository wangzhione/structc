#ifndef _H_CHEAD
#define _H_CHEAD

#include <g2u8.h>
#include <assext.h>
#include <thread.h>

#ifdef _MSC_VER

#include <conio.h>

// locals - 本地字符串特殊处理, winds 会把 utf8 转 gbk
inline char * locals(char utf8s[]) {
    if (isu8s(utf8s)) {
        u82g(utf8s);
    }
    return utf8s;
}

inline void cls(void) {
    system("cls");
}

#endif

#ifdef __GNUC__

#include <unistd.h>
#include <termios.h>

inline char * locals(char utf8s[]) {
    return utf8s;
}

// cls - 屏幕清除宏, 依赖系统脚本
inline void cls(void) {
    printf("\ec");
}

// getch - 立即得到用户输入的一个字符
inline int getch(void) {
    int cr;
    struct termios nts, ots;
    if (tcgetattr(0, &ots)) // 得到当前终端(0表示标准输入)的设置
        return EOF;

    nts = ots;
    cfmakeraw(&nts); // 设置终端为 Raw 原始模式，模式下输入数据全以字节单位被处理
    if (tcsetattr(0, TCSANOW, &nts)) // 设置上更改之后的设置
        return EOF;

    cr = getchar();
    if (tcsetattr(0, TCSANOW, &ots)) // 设置还原成老的模式
        return EOF;
    return cr;
}

#endif

// epause - 程序结束等待操作
inline void epause(void) {
    rewind(stdin);
    fflush(stderr); fflush(stdout);
    printf("Press any key to continue . . .");
    getch();
}

//
// STR - 添加双引号的宏 
// v    : 待添加双引号的量
//
#define STR(v) S_R(v)
#define S_R(v) #v

//
// LEN - 获取数组长度
// arr : 数组名
//
#define LEN(a) sizeof(a)/sizeof(*(a))

//
// EXTERN_RUN - 简单的声明, 并立即使用的宏
// ftest    : 需要执行的函数名称
// ...      : 可变参数, 保留
//
#define EXTERN_RUN(ftest, ...)                                      \
do {                                                                \
    extern void ftest();                                            \
    ftest (__VA_ARGS__);                                            \
} while(0)

//
// TEST_CODE - 测试代码块, 并输出简单时间信息
// code : { ... } 包裹的代码块
//
#define TEST_CODE(code)                                             \
do {                                                                \
    clock_t $s = clock();                                           \
    code                                                            \
    double $e = (double)clock();                                    \
    printf("test code run time:%lfs\n", ($e-$s)/CLOCKS_PER_SEC);    \
} while (0)

//
// TEST_FUNC - 用于单元测试函数, 执行并输出运行时间
// ftest    : 需要执行的测试函数名称
// ...      : 可变参数, 保留
//
#define TEST_FUNC(ftest, ...)                                       \
do {                                                                \
    extern void ftest();                                            \
    clock_t $s = clock();                                           \
    ftest (##__VA_ARGS__);                                          \
    double $e = (double)clock();                                    \
    printf(STR(ftest) " run time:%lfs\n", ($e-$s)/CLOCKS_PER_SEC);  \
} while(0)

// hton - 本地字节序转网络字节序(大端)
// noth - 网络字节序转本地字节序
inline uint32_t hton(uint32_t x) {
#ifndef ISBENIAN
    uint8_t t;
    union { uint32_t i; uint8_t s[sizeof(uint32_t)]; } u = { x };
    t = u.s[0]; u.s[0] = u.s[sizeof(u)-1]; u.s[sizeof(u)-1] = t;
    t = u.s[1]; u.s[1] = u.s[sizeof(u)-2]; u.s[sizeof(u)-2] = t;
    return u.i;
#else
    return x;
#endif
}

inline uint32_t ntoh(uint32_t x) {
    return hton(x);
}

#endif//_H_CHEAD
