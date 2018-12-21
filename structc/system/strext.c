#include "strext.h"

//
// str_hash - Brian Kernighan 与 Dennis Ritchie hash 算法
// str      : 字符串内容
// return   : 返回计算后的 hash 值
//
unsigned 
str_hash(const char * str) {
    register unsigned h = 0u;
    if (str) {
        register unsigned c;
        while ((c = *str++))
            h = h * 131u + c;
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
    if (!src || !tar || !n) return EParam;
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

// str_vprintf - 成功直接返回
static char * str_vprintf(const char * format, va_list arg) {
    char buf[BUFSIZ];
    int n = vsnprintf(buf, sizeof buf, format, arg);
    if (n < sizeof buf) {
        char * ret = malloc(n + 1);
        return memcpy(ret, buf, n + 1);
    }
    return NULL;
}

//
// str_printf - 字符串构建函数
// format   : 构建格式参照 printf
// ...      : 参数集
// return   : char * 堆上内存
//
char * 
str_printf(const char * format, ...) {
    char * ret;
    int n, cap;
    va_list arg;
    va_start(arg, format);

    // BUFSIZ 以下内存直接分配
    ret = str_vprintf(format, arg);
    if (ret != NULL)
        return ret;

    cap = BUFSIZ << 1;
    for (;;) {
        ret = malloc(cap);
        n = vsnprintf(ret, cap, format, arg);
        // 失败的情况
        if (n < 0) {
            free(ret);
            return NULL;
        }

        // 成功情况
        if (n < cap)
            break;

        // 内存不足的情况
        free(ret);
        cap <<= 1;
    }

    return realloc(ret, n + 1);
}

//
// str_freads - 读取整个文件内容返回, 需要事后 free
// path     : 文件路径
// return   : 文件内容字符串, NULL 表示读取失败
//
char * 
str_freads(const char * path) {
    size_t n, cap, len;
    char * str, buf[BUFSIZ];
    FILE * txt = fopen(path, "rb");
    if (NULL == txt) return NULL;

    // 读取数据
    n = fread(buf, sizeof(char), BUFSIZ, txt);
    if (n == 0 || ferror(txt)) {
        fclose(txt);
        return NULL;
    }

    // 直接分配内存足够直接返回内容
    if (n < BUFSIZ) {
        fclose(txt);
        str = malloc(n + 1);
        memcpy(str, buf, n);
        str[n] = '\0';
        return str;
    }

    str = malloc((cap = n << 1));
    memcpy(str, buf, len = n);
    do {
        n = fread(buf, sizeof(char), BUFSIZ, txt);
        if (ferror(txt)) {
            fclose(txt);
            free(str);
            return NULL;
        }

        // 填充数据
        if (len + n >= cap)
            str = realloc(str, cap <<= 1);
        memcpy(str + len, buf, n);
        len += n;
    } while (n == BUFSIZ);

    // 设置结尾, 并返回结果
    fclose(txt);
    str[len] = '\0';
    return realloc(str, len + 1);
}

// str_fwrite - 按照约定输出数据到文件中
static int str_fwrite(const char * p, const char * s, const char * m) {
    int len;
    FILE * txt;
    if (!p || !*p || !s || !m)
        return EParam;
    // 打开文件, 写入消息, 关闭文件
    if (!(txt = fopen(p, m)))
        return EFd;

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
