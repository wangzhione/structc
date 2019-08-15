#include "tstr.h"

//
// tstr_expand - 字符串扩容, low level api
// tsr      : 可变字符串
// len      : 扩容的长度
// return   : tsr->str + tsr->len 位置的串
//
char * 
tstr_expand(tstr_t tsr, size_t len) {
    size_t cap = tsr->cap;
    if ((len += tsr->len) > cap) {
        // 走 1.5 倍内存分配, '合理'降低内存占用
        while (cap < len) cap = cap * 3 / 2;
        tsr->str = realloc(tsr->str, cap);
        tsr->cap = cap;
    }
    return tsr->str + tsr->len;
}

//
// tstr_delete - tstr_t 释放函数
// tsr      : 待释放的串结构
// return   : void
//
inline void 
tstr_delete(tstr_t tsr) {
    free(tsr->str);
    free(tsr);
}

//
// tstr_t 创建函数, 根据 C 串创建 tstr_t 字符串
// str      : 待创建的字符串
// len      : 创建串的长度
// return   : 返回创建的字符串
//
inline tstr_t 
tstr_create(const char * str, size_t len) {
    tstr_t tsr = malloc(sizeof(struct tstr));
    tsr->str = malloc(TSTR_INT);
    tsr->cap = TSTR_INT;
    tsr->len = 0;
    if (str && len) tstr_appendn(tsr, str, len);
    return tsr;
}

inline tstr_t 
tstr_creates(const char * str) {
    tstr_t tsr = malloc(sizeof(struct tstr));
    tsr->str = malloc(TSTR_INT);
    tsr->cap = TSTR_INT;
    tsr->len = 0;
    if (str) tstr_appends(tsr, str);
    return tsr;
}

//
// tstr_t 串结构中添加字符等
// tsr      : tstr_t 串
// c        : 添加 char
// str      : 添加 char *
// sz       : 添加串的长度
// return   : void
//
inline void 
tstr_appendc(tstr_t tsr, int c) {
    // 这类函数不做安全检查, 为了性能
    tstr_expand(tsr, 1);
    tsr->str[tsr->len++] = c;
}

inline void 
tstr_appends(tstr_t tsr, const char * str) {
    if (tsr && str) {
        unsigned sz = (unsigned)strlen(str);
        if (sz > 0)
            tstr_appendn(tsr, str, sz);
        tstr_cstr(tsr);
    }
}

inline void 
tstr_appendn(tstr_t tsr, const char * str, size_t sz) {
    tstr_expand(tsr, sz);
    memcpy(tsr->str + tsr->len, str, sz);
    tsr->len += sz;
}

//
// tstr_cstr - 通过 str_t 串得到一个 C 串以'\0'结尾
// tsr      : tstr_t 串
// return   : 返回构建 C 串, 内存地址 tsr->str
//
inline char * 
tstr_cstr(tstr_t tsr) {
    if (tsr->len < 1u || tsr->str[tsr->len - 1]) {
        tstr_expand(tsr, 1u);
        tsr->str[tsr->len] = '\0';
    }
    return tsr->str;
}

//
// tstr_dupstr - 得到 C 的串, 需要自行 free
// tsr      : tstr_t 串
// return   : 返回创建好的 C 串
//
inline char * 
tstr_dupstr(tstr_t tsr) {
    if (tsr && tsr->len >= 1) {
        // 构造内存, 返回最终结果
        size_t len = tsr->len + !!tsr->str[tsr->len - 1];
        char * str = malloc(len * sizeof(char));
        memcpy(str, tsr->str, len - 1);
        str[len - 1] = '\0';
        return str;
    }
    return NULL;
}

//
// tstr_popup - 字符串头弹出 len 长度字符
// tsr      : 可变字符串
// len      : 弹出的长度
// return   : void
//
inline void 
tstr_popup(tstr_t tsr, size_t len) {
    if (len >= tsr->len)
        tsr->len = 0;
    else {
        tsr->len -= len;
        memmove(tsr->str, tsr->str + len, tsr->len);
    }
}

//
// tstr_printf - 参照 sprintf 方式填充内容
// tsr      : tstr_t 串
// format   : 待格式化的串
// ...      : 等待进入的变量
// return   : 返回创建的 C 字符串内容
//
char * 
tstr_printf(tstr_t tsr, const char * format, ...) {
    // 确定待分配内存 size
    va_list arg;
    va_start(arg, format);
    int n = vsnprintf(NULL, 0, format, arg);
    va_end(arg);

    if (n <= 0) 
        return tstr_cstr(tsr);

    // 预备待分配内存
    tstr_expand(tsr, ++n);

    // 数据转换, 预设 vsnprintf 二次安全
    va_start(arg, format);
    tsr->len += vsnprintf(tsr->str + tsr->len, n, format, arg);
    va_end(arg);

    return tsr->str;
}
