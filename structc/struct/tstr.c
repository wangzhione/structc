#include <tstr.h>

// 字符串构建的初始化大小
#define _UINT_TSTR  (32u)

//
// tstr_expand - 为当前字符串扩容, 属于低级api
// tstr		: 可变字符串
// len		: 扩容的长度
// return	: tstr->str + tstr->len 位置的串
//
char * 
tstr_expand(tstr_t tstr, size_t len) {
    size_t cap = tstr->cap;
    if ((len += tstr->len) > cap) {
        for (cap = cap < _UINT_TSTR ? _UINT_TSTR : cap; cap < len; cap <<= 1)
            ;
        // 需要重新分配内存
        tstr->str = realloc(tstr->str, cap);
        tstr->cap = cap;
    }
    return tstr->str + tstr->len;
}

//
// tstr_popup - 从字符串头弹出len长度字符
// tstr		: 可变字符串
// len		: 弹出的长度
// return	: void
//
void 
tstr_popup(tstr_t tstr, size_t len) {
    if (len > tstr->len)
        tstr->len = 0;
    else {
        tstr->len -= len;
        memmove(tstr->str, tstr->str + len, tstr->len);
    }
}

//
// tstr_dupstr - 得到一个精简的c的串, 需要自己事后free
// tstr		: tstr_t 串
// return	: 返回创建好的c串
//
char * 
tstr_dupstr(tstr_t tstr) {
    if (tstr && tstr->len >= 1) {
        // 构造内存, 返回最终结果
        size_t len = tstr->len + !!tstr->str[tstr->len - 1];
        char * str = malloc(len * sizeof(char));
        memcpy(str, tstr->str, len - 1);
        str[len - 1] = '\0';
        return str;
    }
    return NULL;
}

//
// tstr_t 创建函数, 会根据c的tstr串创建一个tstr_t结构的字符串
// str		: 待创建的字符串
// len		: 创建串的长度
// return	: 返回创建好的字符串,内存不足会打印日志退出程序
//
tstr_t 
tstr_create(const char * str, size_t len) {
    tstr_t tstr = calloc(1, sizeof(struct tstr));
    if (str && len > 0)
        tstr_appendn(tstr, str, len);
    return tstr;
}

tstr_t 
tstr_creates(const char * str) {
    tstr_t tstr = calloc(1, sizeof(struct tstr));
    if (str)
        tstr_appends(tstr, str);
    return tstr;
}

//
// tstr_delete - tstr_t 释放函数
// tstr		: 待释放的串结构
// return   : void
//
inline void 
tstr_delete(tstr_t tstr) {
    free(tstr->str);
    free(tstr);
}

//
// 向tstr_t串结构中添加字符等, 内存分配失败内部会自己处理
// c		: 单个添加的char
// str		: 添加的c串
// sz		: 添加串的长度
//
inline void 
tstr_appendc(tstr_t tstr, int c) {
    // 这类函数不做安全检查, 为了性能
    tstr_expand(tstr, 1);
    tstr->str[tstr->len++] = c;
}

void 
tstr_appends(tstr_t tstr, const char * str) {
    if (tstr && str) {
        unsigned sz = (unsigned)strlen(str);
        if (sz > 0)
            tstr_appendn(tstr, str, sz);
        tstr_cstr(tstr);
    }
}

inline void 
tstr_appendn(tstr_t tstr, const char * str, size_t sz) {
    tstr_expand(tstr, sz);
    memcpy(tstr->str + tstr->len, str, sz);
    tstr->len += sz;
}

//
// tstr_cstr - 通过cstr_t串得到一个c的串以'\0'结尾
// tstr		: tstr_t 串
// return	: 返回构建好的c的串, 内存地址tstr->str
//
char * 
tstr_cstr(tstr_t tstr) {
    if (tstr->len < 1u || tstr->str[tstr->len - 1]) {
        tstr_expand(tstr, 1u);
        tstr->str[tstr->len] = '\0';
    }
    return tstr->str;
}

// _tstr_printf : BUFSIZ 以下内存处理
static int _tstr_printf(tstr_t tstr, const char * format, va_list arg) {
    char buf[BUFSIZ];
    int len = vsnprintf(buf, sizeof buf, format, arg);
    if (len < sizeof buf) {
        // 合法直接构建内存返回
        if (len >= 0)
            tstr_appendn(tstr, buf, len);
        tstr_cstr(tstr);
    }
    return len;
}

//
// tstr_printf - 参照 sprintf 填充方式写入内容
// tstr     : tstr_t 串
// format   : 待格式化的串
// ...      : 等待进入的变量
// return   : 返回创建好的C字符串内容
//
char * 
tstr_printf(tstr_t tstr, const char * format, ...) {
    int cap;
    va_list arg;
    va_start(arg, format);

    // 初步构建失败直接返回
    cap = _tstr_printf(tstr, format, arg);
    if (cap < BUFSIZ)
        return tstr->str;
    
    // 开始详细构建内存
    for (;;) {
        char * ret = malloc(cap <<= 1);
        int len = vsnprintf(ret, cap, format, arg);
        // 失败的情况, 这里没有打印错误信息. 需要上层自己处理
        if (len < 0) {
            free(ret);
            break;
        }

        // 成功情况, 插入内存数据
        if (len < cap) {
            tstr_appendn(tstr, ret, len);
            break;
        }

        // 重新构建内存
        free(ret);
    }

    return tstr_cstr(tstr);
}
