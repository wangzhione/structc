#ifndef _H_CHEAD
#define _H_CHEAD

#include <g2u8.h>
#include <thread.h>

//
// STR - 添加双引号的宏 
// v    : 待添加双引号的量
//
#define S_R(v) #v
#define STR(v) S_R(v)

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
#define EXTERN_RUN(ftest, ...)                                          \
do {                                                                    \
    extern void ftest();                                                \
    ftest (__VA_ARGS__);                                                \
} while(0)

//
// TEST_CODE - 测试代码块, 并输出简单时间信息
// code : { ... } 包裹的代码块
//
#define TEST_CODE(code)                                                 \
do {                                                                    \
    clock_t $s = clock();                                               \
    code                                                                \
    double $e = (double)clock();                                        \
    printf("test code run time:%lfs.\n", ($e - $s) / CLOCKS_PER_SEC);   \
} while (0)

//
// TEST_FUNC - 用于单元测试函数, 执行并输出运行时间
// ftest    : 需要执行的测试函数名称
// ...      : 可变参数, 保留
//
#define TEST_FUNC(ftest, ...)                                           \
do {                                                                    \
    extern void ftest();                                                \
    clock_t $s = clock();                                               \
    ftest (##__VA_ARGS__);                                              \
    double $e = (double)clock();                                        \
    printf(STR(ftest) " run time:%lfs.\n", ($e - $s) / CLOCKS_PER_SEC); \
} while(0)

//
// os_cstr - 字符串串特殊处理, winds gbk, linux utf8
// utf8s    : utf8s 串
// return   : gbks 串内容
//
inline char * os_cstr(char utf8s[]) {
    if (isu8s(utf8s)) {
        u82g(utf8s);
    }
    return utf8s;
}

#endif//_H_CHEAD
