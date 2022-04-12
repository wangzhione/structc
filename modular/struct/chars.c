#include "chars.h"

// CHARS_INT 构建字符串初始化大小
#define CHARS_INT    (1 << 7)

//
// chars_expand - low level 字符串扩容 api
// cs       : 可变字符串
// len      : 扩容的长度
// return   : cstr::str + cstr::len 位置的串
//
char * 
chars_expand(struct chars * cs, size_t len) {
    size_t cap = cs->cap;
    if ((len += cs->len) > cap) {
        if (cap < CHARS_INT) {
            cap = CHARS_INT;
        } else {
            // 排脑门走 1.5 倍内存分配, '合理'降低内存占用
            while (cap < len) 
                cap = cap * 3 / 2;
        }

        cs->str = realloc(cs->str, cs->cap = cap);
    }
    return cs->str + cs->len;
}

//
// chars_dup - 得到 C 堆上的串, 需要自行 free
// cs       : struct chars * 串
// return   : 返回创建好的 C 串
//
inline char * chars_dup(struct chars * cs) {
    // 构造内存, 返回最终结果
    size_t len = cs->len + (cs->len == 0 || cs->str[cs->len-1] != 0);
    char * str = malloc(len);
    memcpy(str, cs->str, len-1);
    str[len-1] = 0;
    return str;
}

//
// chars_pop - 字符串头弹出 len 长度字符
// cs       : 可变字符串
// len      : 弹出的长度
// return   : void
//
inline void chars_pop(struct chars * cs, size_t len) {
    if (len >= cs->len)
        cs->len = 0;
    else {
        cs->len -= len;
        memmove(cs->str, cs->str+len, cs->len);
    }
}

//
// chars_sprintf - 参照 sprintf 方式填充内容
// cs       : struct chars * 串
// fmt      : 待格式化的串
// ...      : 可变参数列表
// return   : 返回创建的 C 字符串内容
//
char * 
chars_sprintf(struct chars * cs, const char * fmt, ...) {
    // 确定待分配内存 size
    va_list arg;
    va_start(arg, fmt);
    int n = vsnprintf(NULL, 0, fmt, arg);
    va_end(arg);

    if (n <= 0) 
        return chars_get(cs);

    // 获取待分配内存, 尝试填充格式化数据
    chars_expand(cs, ++n);

    va_start(arg, fmt);
    n = vsnprintf(cs->str + cs->len, n, fmt, arg);
    va_end(arg);

    if (n <= 0) 
        return chars_get(cs);

    cs->len += n;
    return cs->str;
}
