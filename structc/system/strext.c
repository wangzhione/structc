#include "strext.h"

//
// str_hash - Brian Kernighan 与 Dennis Ritchie hash 算法
// str      : 字符串内容
// return   : 返回计算后的 hash 值
//
unsigned 
str_hash(const char * str) {
    register unsigned u, h = 0;
    if (str) {
        while ((u = *str++))
            h = h * 131u + u;
    }
    return h;
}

//
// str_cpyn - tar 复制内容到 src 中
// src      : 返回保存内容
// tar      : 目标内容
// n        : 最大容量
// return   : 返回字符串长度
//
int 
str_cpyn(char * src, const char * tar, size_t n) {
    size_t i;
    if (!src || !tar || !n) return -1;
    for (i = 1; 
        (i < n) && (*src++ = *tar++); ++i)
        ;
    if (i == n) *src = '\0';
    return (int)i - 1;
}

//
// str_cmpi - 字符串不区分大小写比较函数
// ls       : 左串
// rs       : 右串
// return   : ls > rs 返回 > 0; ... < 0; ... =0
//
int 
str_cmpi(const char * ls, const char * rs) {
    int l, r;
    if (!ls || !rs) return (int)(ls - rs);
    do {
        if ((l = *ls++) >= 'A' && l <= 'Z')
            l += 'a' - 'A';
        if ((r = *rs++) >= 'A' && r <= 'Z')
            r += 'a' - 'A';
    } while (l == r && l);
    return l - r;
}

//
// str_cmpin - 字符串不区分小写的限定字符比较函数
// ls       : 左串
// rs       : 右串
// n        : 长度
// return   : ls > rs 返回 > 0; ... < 0; ... =0
//
int 
str_cmpin(const char * ls, const char * rs, size_t n) {
    int l, r;
    if (!ls || !rs || !n) return (int)(ls - rs);
    do {
        if ((l = *ls++) >= 'A' && l <= 'Z')
            l += 'a' - 'A';
        if ((r = *rs++) >= 'A' && r <= 'Z')
            r += 'a' - 'A';
    } while (--n > 0 && l == r && l);
    return l - r;
}

//
// str_trim - 去除字符数组前后控制字符
// str      : 待操作的字符数组 \0 结尾
// return   : 返回构建好字符数组首地址
//
char * 
str_trim(char str[]) {
    char * s, * e;
    if (!str || !*str)
        return str;

    // 找到第一个不是空格字符的地址
    for (s = str; isspace(*s); ++s)
        ;

    // 找到最后一个不是空格字符的地址
    e = s + strlen(s) - 1;
    if (isspace(*e)) {
        do --e; while (isspace(*e));
        e[1] = '\0';
    }

    // 开始返回移动后的首地址
    return s == str ? str : memmove(str, s, e - s + 2);
}

//
// str_sprintf - 格化式字符串构建
// fmt      : 构建格式参照 printf
// ...      : 参数集
// return   : char * 堆上内存
//
char * 
str_sprintf(const char * fmt, ...) {
    // 确定待分配内存 size
    va_list arg;
    va_start(arg, fmt);
    int n = vsnprintf(NULL, 0, fmt, arg);
    va_end(arg);

    if (n < 0) 
        return NULL;

    // 获取待分配内存, 尝试填充格式化数据
    char * ret = malloc(++n);

    va_start(arg, fmt);
    n = vsnprintf(ret, n, fmt, arg);
    va_end(arg);

    if (n < 0) {
        free(ret);
        return NULL;
    }

    return ret;
}

//
// str_freads - 读取整个文件内容返回, 需要事后 free
// path     : 文件路径
// return   : 文件内容字符串, NULL 表示读取失败
//
char * 
str_freads(const char * path) {
    int64_t size = fsize(path);
    if (size < 0)
        return NULL;
    if (size == 0) 
        return calloc(1, sizeof (char));

    // 尝试打开文件读取处理
    FILE * txt = fopen(path, "rb");
    if (!txt) 
        return NULL;

    // 构建最终内存
    char * str = malloc(size + 1);
    str[size] = '\0';

    size_t n = fread(str, sizeof(char), size, txt);
    assert(n == (size_t)size);
    if (ferror(txt)) {
        free(str);
        fclose(txt);
        return NULL;
    }

    fclose(txt);

    return str;
}

// str_fwrite - 按照约定输出数据到文件中
static int str_fwrite(const char * p, const char * s, const char * m) {
    int len;
    FILE * txt;
    if (!p || !*p || !s || !m) {
        return -EINVAL; // 参数无效
    }

    // 打开文件, 写入消息, 关闭文件
    if (!(txt = fopen(p, m))) {
        return -ENOENT; // 文件或路径不存在
    }

    len = fputs(s, txt);
    fclose(txt);
    // 输出文件长度
    return len;
}

//
// str_fwrites - 将 C 串 str 覆盖写到 path 文件中
// path     : 文件路径
// str      : C 串内容
// return   : >=0 is success, < 0 is error
//
inline int 
str_fwrites(const char * path, const char * str) {
    return str_fwrite(path, str, "wb");
}

//
// str_fappends - 将 C 串 str 追加写到 path 文件末尾
// path     : 文件路径
// str      : C 串内容
// return   : >=0 is success, < 0 is error
//
inline int 
str_fappends(const char * path, const char * str) {
    return str_fwrite(path, str, "ab");
}
