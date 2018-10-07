#include "strext.h"

//
// str_hash - Brian Kernighan与 Dennis Ritchie 简便快捷的 hash算法
// str      : 字符串内容
// return   : 返回计算后的hash值
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
    if (!src || !tar || n == 0)
        return -2;
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
    if (!ls || !rs || n < 1) return (int)(ls - rs);

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

// _str_printf : 成功直接返回
static char * str_vprintf(const char * format, va_list arg) {
    char buf[BUFSIZ];
    int len = vsnprintf(buf, sizeof buf, format, arg);
    if (len < sizeof buf) {
        char * ret = malloc(len + 1);
        return memcpy(ret, buf, len + 1);
    }
    return NULL;
}

//
// str_printf - 字符串构建函数
// format   : 构建格式参照pritnf
// ...      : 参数集
// return   : char * 堆上内存
//
char * 
str_printf(const char * format, ...) {
    char * ret;
    int len, cap;
    va_list arg;
    va_start(arg, format);

    // BUFSIZ 以下内存直接分配
    ret = str_vprintf(format, arg);
    if (ret != NULL)
        return ret;

    cap = BUFSIZ << 1;
    for (;;) {
        ret = malloc(cap);
        len = vsnprintf(ret, cap, format, arg);
        // 失败的情况
        if (len < 0) {
            free(ret);
            return NULL;
        }

        // 成功情况
        if (len < cap)
            break;

        // 内存不足的情况
        free(ret);
        cap <<= 1;
    }

    return realloc(ret, len + 1);
}

//
// str_freads - 简单的文件读取类,会读取完毕这个文件内容返回, 需要自己free
// path     : 文件路径
// return   : 创建好的字符串内容, 返回NULL表示读取失败
//
char * 
str_freads(const char * path) {
    size_t rn, cap, len;
    char * str, buf[BUFSIZ];
    FILE * txt = fopen(path, "rb");
    if (NULL == txt) return NULL;

    // 读取数据
    rn = fread(buf, sizeof(char), BUFSIZ, txt);
    if (rn == 0 || ferror(txt)) {
        fclose(txt);
        return NULL;
    }

    // 直接分配内存足够直接返回内容
    if (rn < BUFSIZ) {
        fclose(txt);
        str = malloc(rn + 1);
        memcpy(str, buf, rn);
        str[rn] = '\0';
        return str;
    }

    str = malloc((cap = rn << 1));
    memcpy(str, buf, len = rn);
    do {
        rn = fread(buf, sizeof(char), BUFSIZ, txt);
        if (ferror(txt)) {
            fclose(txt);
            free(str);
            return NULL;
        }

        // 填充数据
        if (len + rn >= cap)
            str = realloc(str, cap <<= 1);
        memcpy(str + len, buf, rn);
        len += rn;
    } while (rn == BUFSIZ);

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
        return -2;
    // 打开文件, 写入消息, 关闭文件
    if (!(txt = fopen(p, m)))
        return -3;

    len = fputs(s, txt);
    fclose(txt);
    // 输出文件长度
    return len;
}

//
// str_fwrites - 将c串str覆盖写入到path路径的文件中
// path     : 文件路径
// str      : c的串内容
// return   : >=0 is success, < 0 is error
//
inline int 
str_fwrites(const char * path, const char * str) {
    return str_fwrite(path, str, "wb");
}

//
// str_fappends - 将c串str写入到path路径的文件中末尾
// path     : 文件路径
// str      : c的串内容
// return   : >=0 is success, < 0 is error
//
inline int 
str_fappends(const char * path, const char * str) {
    return str_fwrite(path, str, "ab");
}
