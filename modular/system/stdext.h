#pragma once

#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "alloc.h"

#if defined(__linux__) && defined(__GNUC__)

#include <unistd.h>
#include <termios.h>

//
// mkdir - 单层目录创建函数宏, 类比 mkdir path
// path     : 目录路径
// return   : 0 表示成功, -1 表示失败, errno 存原因
// 
#undef  mkdir
#define mkdir(path)                                 \
mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)

// getch - 立即得到用户输入的一个字符
inline int getch(void) {
    struct termios now, old;
    // 得到当前终端标准输入的设置
    if (tcgetattr(STDIN_FILENO, &old))
        return EOF;
    now = old;

    // 设置终端为 Raw 原始模式，让输入数据全以字节单位被处理
    cfmakeraw(&now);
    // 设置上更改之后的设置
    if (tcsetattr(STDIN_FILENO, TCSANOW, &now))
        return EOF;

    int c = getchar();

    // 设置还原成老的模式
    if (tcsetattr(STDIN_FILENO, TCSANOW, &old))
        return EOF;
    return c;
}

// cls - 屏幕清除, 依赖系统脚本
inline void cls(void) { printf("\ec"); }

#endif

#if defined(_WIN32) && defined(_MSC_VER)

#include <io.h>
#include <conio.h>
#include <direct.h>
#include <windows.h>

// int access(const char * path, int mode /* 四个检测宏 */);
#ifndef     F_OK
#  define   F_OK    (0)
#endif
#ifndef     X_OK
#  define   X_OK    (1)
#endif       
#ifndef     W_OK
#  define   W_OK    (2)
#endif       
#ifndef     R_OK
#  define   R_OK    (4)
#endif

// cls - 屏幕清除, 依赖系统脚本
inline void cls(void) { system("cls"); }

#endif

//
// fmtime - 得到文件最后修改时间
// path     : 文件路径
// return   : 返回时间戳, -1 表示失败
//
inline time_t fmtime(const char * path) {
    struct stat st;
    // 数据最后的修改时间
    return stat(path, &st) ? -1 : st.st_mtime;
}

//
// fsize - 得到文件内容内存大小
// path     : 文件路径
// return   : 返回文件内存
//
inline int64_t fsize(const char * path) {
    struct stat st;
    // 数据最后的修改时间
    return stat(path, &st) ? -1 : st.st_size;
}

//
// removes - 删除非空目录 or 文件
// path     : 文件路径
// return   : < 0 is error, >=0 is success
//
extern int removes(const char * path);

//
// mkdirs - 创建多级目录
// path     : 目录路径
// return   : < 0 is error, 0 is success
//
extern int mkdirs(const char * path);

//
// fmkdir - 通过文件路径创建目录
// path     : 文件路径
// return   : < 0 is error, 0 is success
//
extern int fmkdir(const char * path);

//
// getawd - 得到程序运行目录, \\ or / 结尾
// buf      : 存储地址
// size     : 存储大小
// return   : 返回长度, -1 or >= size is unusual 
//
extern int getawd(char * buf, size_t size);
