#ifndef _FILE_H
#define _FILE_H

#include "atom.h"
#include "struct.h"
#include "strext.h"

#ifdef __GNUC__

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

//
// mkdir - 单层目录创建函数宏, 类比 mkdir path
// path     : 目录路径
// return   : 0 表示成功, -1 表示失败, 失败原因见 errno
// 
#undef  mkdir
#define mkdir(path)                                 \
mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)

//
// mtime - 得到文件最后修改时间
// path     : 文件路径
// return   : 返回时间戳, -1 表示失败
//
inline time_t mtime(const char * path) {
    struct stat st;
    // 数据最后的修改时间
    return stat(path, &st) ? -1 : st.st_mtime;
}

#endif

#ifdef _MSC_VER

#include <io.h>
#include <direct.h>
#include <windows.h>

// int access(const char * path, int mode /* 四个检测宏 */);
#ifndef F_OK
#   define  F_OK    (0)
#endif       
#ifndef X_OK 
#   define  X_OK    (1)
#endif       
#ifndef W_OK 
#   define  W_OK    (2)
#endif       
#ifndef R_OK 
#   define  R_OK    (4)
#endif

inline time_t mtime(const char * path) {
    WIN32_FILE_ATTRIBUTE_DATA st;
    if (!GetFileAttributesEx(path, GetFileExInfoStandard, &st))
        return -1;
    // 基于 winds x64 sizeof(long) = 4
    return *(time_t *)&st.ftLastWriteTime;
}

#endif

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
// mkfdir - 通过文件路径创建目录
// path     : 文件路径
// return   : < 0 is error, 0 is success
//
extern int mkfdir(const char * path);

//
// getawd - 得到程序运行目录, \\ or / 结尾
// buf      : 存储地址
// size     : 存储大小
// return   : 返回长度, -1 is error 
//
extern int getawd(char * buf, size_t size);

//
// file_f - 文件更新行为
//
typedef void (* file_f)(FILE * c, void * arg);

//
// file_set - 文件注册更新行为
// path     : 文件路径
// func     : NULL 是标记清除, 否则 update -> func(path -> FILE, arg)
// arg      : func 额外参数
// return   : void
//
extern void file_set(const char * path, file_f func, void * arg);

//
// file_update - 配置文件刷新操作
// return   : void
//
extern void file_update(void);

#endif//_FILE_H
