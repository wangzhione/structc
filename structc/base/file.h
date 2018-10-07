#ifndef _H_FILE
#define _H_FILE

#include "struct.h"
#include "strext.h"

#ifdef __GNUC__

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

//
// mkdir - 通用的单层目录创建函数宏, 等价于 mkdir path
// path     : 目录路径加名称
// return   : 0表示成功, -1表示失败, 失败原因都在 errno
// 
#undef  mkdir
#define mkdir(path)                                 \
mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)

//
// mtime - 得到文件最后修改时间
// path     : 文件名称
// return   : 返回时间戳, -1 表示错误
//
inline time_t mtime(const char * path) {
    struct stat ss;
    // 数据最后的修改时间
    return stat(path, &ss) ? -1 : ss.st_mtime;
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
    WIN32_FILE_ATTRIBUTE_DATA wfad;
    if (!GetFileAttributesEx(path, GetFileExInfoStandard, &wfad))
        return -1;
    // 基于 winds x64 sizeof(long) = 4
    return *(time_t *)&wfad.ftLastWriteTime;
}

#endif

//
// removes - 删除非空目录 or 文件
// path     : 文件全路径
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
// getawd - 得到程序所在目录绝对路径会带上 \\ or /
// buf      : 存储地址
// size     : 存储大小
// return   : NULL is error or buf is success
//
extern char * getawd(char * buf, size_t size);

//
// :0 一个和程序同生存的配置文件动态刷新机制
// file_f - 文件更新行为
//
typedef void (* file_f)(FILE * c, void * arg);

//
// file_set - 文件注册触发行为
// path     : 文件路径
// func     : file update -> func(path -> FILE, arg), func is NULL 标记清除
// arg      : 注入的额外参数
// return   : void
//
extern void file_set(const char * path, file_f func, void * arg);

//
// file_update - 更新注册配置解析事件
// return   : void
//
extern void file_update(void);

#endif//_H_FILE
