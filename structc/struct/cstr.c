#include "cstr.h"

//
// cstr_expand - low level 字符串扩容 api
// cs       : 可变字符串
// len      : 扩容的长度
// return   : cstr::str + cstr::len 位置的串
//
char * 
cstr_expand(cstr_t cs, size_t len) {
    size_t cap = cs->cap;
    if ((len += cs->len) > cap) {
        // 走 1.5 倍内存分配, '合理'降低内存占用
        while (cap < len) 
            cap = cap * 3 / 2;

        cs->str = realloc(cs->str, cs->cap = cap);
    }
    return cs->str + cs->len;
}

//
// cstr_delete - cstr_t 释放函数
// cs       : 待释放的串对象
// return   : void
//
inline void 
cstr_delete(cstr_t cs) {
    free(cs->str);
    free(cs);
}

//
// cstr_create - cstr_t 创建函数, 根据 C 串创建 cstr_t 字符串
// str      : 待创建的字符串
// len      : 创建串的长度
// return   : 返回创建的字符串
//
inline cstr_t 
cstr_creats(const char * str) {
    cstr_t cs = malloc(sizeof(struct cstr));
    cs->str = malloc(CSTR_INT);
    cs->cap = CSTR_INT;
    cs->len = 0;
    cstr_appends(cs, str);
    return cs;
}

inline cstr_t 
cstr_create(const char * str, size_t len) {
    cstr_t cs = malloc(sizeof(struct cstr));
    cs->str = malloc(CSTR_INT);
    cs->cap = CSTR_INT;
    cs->len = 0;
    if (str && len) cstr_appendn(cs, str, len);
    return cs;
}

//
// cstr_t 串结构中添加字符等
// cs       : cstr_t 串
// c        : 添加 char
// str      : 添加 char *
// len      : 添加串的长度
// return   : void
//
inline void 
cstr_appendc(cstr_t cs, int c) {
    // 这类函数不做安全检查, 为了性能
    cstr_expand(cs, 1);
    cs->str[cs->len++] = c;
}

inline void 
cstr_appends(cstr_t cs, const char * str) {
    if (cs && str) {
        size_t sz = strlen(str);
        if (sz > 0)
            cstr_appendn(cs, str, sz);
        cstr_get(cs);
    }
}

inline void 
cstr_appendn(cstr_t cs, const char * str, size_t len) {
    memcpy(cstr_expand(cs, len), str, len);
    cs->len += len;
}

//
// cstr_get - 通过 str_t 串得到一个 C 串以'\0'结尾
// cs       : cstr_t 串
// return   : 返回构建 C 串, 内存地址 cs->str
//
inline char * 
cstr_get(cstr_t cs) {
    *cstr_expand(cs, 1) = '\0';
    return cs->str;
}

//
// cstr_dup - 得到 C 堆上的串, 需要自行 free
// cs       : cstr_t 串
// return   : 返回创建好的 C 串
//
inline char * 
cstr_dup(cstr_t cs) {
    // 构造内存, 返回最终结果
    size_t len = cs->len + (!cs->len||cs->str[cs->len-1]);
    char * str = malloc(len * sizeof(char));
    memcpy(str, cs->str, len - 1);
    str[len - 1] = '\0';
    return str;
}

//
// cstr_popup - 字符串头弹出 len 长度字符
// cs       : 可变字符串
// len      : 弹出的长度
// return   : void
//
inline void 
cstr_popup(cstr_t cs, size_t len) {
    if (len >= cs->len)
        cs->len = 0;
    else {
        cs->len -= len;
        memmove(cs->str, cs->str + len, cs->len);
    }
}

//
// cstr_sprintf - 参照 sprintf 方式填充内容
// cs       : cstr_t 串
// fmt      : 待格式化的串
// ...      : 可变参数列表
// return   : 返回创建的 C 字符串内容
//
char * 
cstr_sprintf(cstr_t cs, const char * fmt, ...) {
    // 确定待分配内存 size
    va_list arg;
    va_start(arg, fmt);
    int n = vsnprintf(NULL, 0, fmt, arg);
    va_end(arg);

    if (n <= 0) 
        return cstr_get(cs);

    // 获取待分配内存, 尝试填充格式化数据
    cstr_expand(cs, ++n);

    va_start(arg, fmt);
    n = vsnprintf(cs->str + cs->len, n, fmt, arg);
    va_end(arg);

    if (n <= 0) 
        return cstr_get(cs);

    cs->len += n;
    return cs->str;
}
