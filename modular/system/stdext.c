#include "stdext.h"

//
// removes - 删除非空目录 or 文件
// path     : 文件路径
// return   : < 0 is error, >=0 is success
//
inline int removes(const char * path) {
    char s[BUFSIZ];

# ifndef RMRF_STR
#   if defined(_WIN32) && defined(_MSC_VER)
#     define RMRF_STR    "rmdir /s /q \"%s\""
#   else
#     define RMRF_STR    "rm -rf '%s'"
#   endif
# endif

    // path 超过缓冲区长度, 返回异常
    if (snprintf(s, sizeof s, RMRF_STR, path) == sizeof s) 
        return -1;
    return access(path, F_OK) ? 0 : -system(s);
}

//
// mkdirs - 创建多级目录
// path     : 目录路径
// return   : < 0 is error, 0 is success
//
int 
mkdirs(const char * path) {
    char c, * p, * s;

    // 参数错误直接返回
    if (!path || !*path) return -1;

    // 文件存在 or 文件一次创建成功 直接返回
    if (!access(path, F_OK) || !mkdir(path))
        return 0;

    // 跳过第一个 ['/'|'\\'] 检查是否是多级目录
    p = (char *)path;
    while ((c = *++p) != '\0')
        if (c == '/' || c == '\\')
            break;
    if (c == '\0') return -1;

    // 开始循环构建多级目录
    s = p = strdup(path);
    while ((c = *++p) != '\0') {
        if (c == '/' || c == '\\') {
            *p = '\0';

            if (access(s, F_OK)) {
                // 文件不存在, 开始创建, 创建失败直接返回错误
                if (mkdir(s)) {
                    free(s);
                    return -1;
                }
            }

            *p = c;
        }
    }

    // 最后善尾
    c = p[-1]; free(s);
    if (c == '/' || c == '\\')
        return 0;

    // 剩下最后文件路径, 开始构建
    return mkdir(path);
}

//
// fmkdir - 通过文件路径创建目录
// path     : 文件路径
// return   : < 0 is error, 0 is success
//
int 
fmkdir(const char * path) {
    const char * r;
    char c, * p, * s;
    if (!path) return -1;

    for (r = path + strlen(path); r >= path; --r)
        if ((c = *r) == '/' || c == '\\')
            break;
    if (r < path) return -1;

    // 复制地址地址并构建
    s = p = strdup(path);
    p[r - path] = '\0';

    while ((c = *++p) != '\0') {
        if (c == '/' || c == '\\') {
            *p = '\0';

            if (access(s, F_OK)) {
                // 文件不存在, 开始创建, 创建失败直接返回错误
                if (mkdir(s)) {
                    free(s);
                    return -1;
                }
            }

            *p = c;
        }
    }

    // 一定不是 / or \\ 结尾直接, 构建返回
    if (access(s, F_OK)) {
        if (mkdir(s)) {
            free(s);
            return -1;
        }
    }
    free(s);
    return 0;
}

//
// getawd - 得到程序运行目录, \\ or / 结尾
// buf      : 存储地址
// size     : 存储大小
// return   : 返回长度, -1 or >= size is unusual 
//
int 
getawd(char * buf, size_t size) {
    char * tail;

# ifndef getawe
#   if defined(_WIN32) && defined(_MSC_VER)
#     define getawe(b, s)    (int)GetModuleFileNameA(NULL, b, (DWORD)s);
#   else
#     define getawe(b, s)    (int)readlink("/proc/self/exe", b, s);
#   endif
# endif

    int r = getawe(buf, size);
    if (r <= 0)    return -1;
    if ((size_t)r >= size) return  r;

    for (tail = buf + r - 1; tail > buf; --tail)
        if ((r = *tail) == '/' || r == '\\')
            break;
    // believe getawe return
    *++tail = '\0';
    return (int)(tail - buf);
}
