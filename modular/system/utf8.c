﻿#include "utf8$table.h"

// utf82unicode - utf8 串转成 unicode, 返回编码值和长度
int utf82unicode(const char * u8s, size_t n, int * ue, int * len) {
    char c = u8s[0];
    if ((c & 0xF8) == 0xF0) { // 4 位
        if ((n < 4) || 
            (u8s[1] & 0xC0) != 0x80 || 
			(u8s[2] & 0xC0) != 0x80 || (u8s[3] & 0xC0) != 0x80) {
            // 编码字节长度不够
            return -2;
        }
        *len = 4;
        *ue = c & 0x07;

        *ue <<= 6;
        *ue |= u8s[1] & 0x3F;

        *ue <<= 6;
        *ue |= u8s[2] & 0x3F;

        *ue <<= 6;
        *ue |= u8s[3] & 0x3F;
    } else if ((c & 0xF0) == 0xE0) { // 3 位
        if ((n < 3) || 
            (u8s[1] & 0xC0) != 0x80 || (u8s[2] & 0xC0) != 0x80)
            return -2;

        *len = 3;
        *ue = c & 0x0F;

        *ue <<= 6;
        *ue |= u8s[1] & 0x3F;

        *ue <<= 6;
        *ue |= u8s[2] & 0x3F;
    } else if ((c & 0xE0) == 0xC0) { // 2 位
        if ((n < 2) || 
            (u8s[1] & 0xC0) != 0x80) 
            return -2;

        *len = 2;
        *ue = c & 0x1F;

        *ue <<= 6;
        *ue |= u8s[1] & 0x3F;
    } else if ((c & 0x80) == 0x00) { // 1 位
        *len = 1;
        *ue = c;
    } else {
        // 第一位编码不合法
        return -1;
    }

    return 0;
}

// utf82gbkn - utf8 转 gbk 算法实现
size_t utf82gbkn(const char * u8s, size_t n, char * gs) {
    int ue, len;
    unsigned gbk;
    size_t ui = 0, gi = 0;

    while (n > ui) {
        //
        // 对于错误编码, 跳过, 不做精细控制. 听之任之
        //
        if (utf82unicode(u8s + ui, n - ui, &ue, &len)) {
            ++ui;
            continue;
        }

        // 65535 以上编码, gbk 编码没有, 我们默认忽略
        if (ue >= (int) (sizeof unicode_gbk_table / sizeof *unicode_gbk_table)) {}
        else {
            gbk = unicode_gbk_table[ue];
            if (len > 1)
                gs[gi++] = gbk >> 8;
            gs[gi++] = gbk & 0xFF;
        }

        ui += len;
    }
    gs[gi] = '\0';
    return gi;
}

// gbk2utf8n - gbk 转 utf8 算法实现
size_t gbk2utf8n(const char * gs, size_t n, char * u8s) {
    unsigned u, c;
    size_t ui = 0, gi = 0;

    while (n > gi) {
        c = gs[gi++] & 0xFF;
        if (c < 0x80)
            u8s[ui++] = gbk_utf8_table[c];
        else {
            if (n < gi + 1)
                break;

            c <<= 8;
            c |= gs[gi++] & 0xFF;
            u = gbk_utf8_table[c];
            if (u < 0xD192)
                u8s[ui++] = u >> 8;
            else {
                u8s[ui++] = u >> 16;
                u8s[ui++] = (u >> 8) & 0xFF;
            }
            u8s[ui++] = u & 0xFF;
        }
    }
    u8s[ui] = '\0';
    return ui;
}

inline void utf82gbk(char d[]) {
    // len utf8 >= len gbk convert
    size_t n = strlen(d);
    char * gs = malloc(n + 1);
    if (gs == nullptr) {
        RETNIL("malloc panic return nullptrptr size = %zu", n + 1);
    }

    size_t m = utf82gbkn(d, n, gs);
    memcpy(d, gs, m + 1);
    free(gs);
}

inline void gbk2utf8(char d[], size_t n) {
    // 2 * len gbk >= len utf8
    char * u8s = malloc(2 * n + 1);
    if (u8s == nullptr) {
        RETNIL("malloc panic return nullptrptr size = %zu", 2 * n + 1);
    }

    size_t m = gbk2utf8n(d, n, u8s);
    if (m <= n)
        memcpy(d, u8s, m + 1);
    else {
        memcpy(d, u8s, n - 1);
        d[n - 1] = '\0';
    }
    free(u8s);
}

// isutf8_local - 判断是否是 utf8 串的临时状态
static bool isutf8_local(unsigned char c, unsigned char * bytes, bool * ascii) {
    // ascii 码最高位为 0, 0xxx xxxx
    if ((c & 0x80)) *ascii = false;

    // 计算字节数
    if (0 == *bytes) {
        if (c >= 0x80) {
            if (c >= 0xFC && c <= 0xFD) *bytes = 6;
            else if (c >= 0xF8) *bytes = 5;
            else if (c >= 0xF0) *bytes = 4;
            else if (c >= 0xE0) *bytes = 3;
            else if (c >= 0xC0) *bytes = 2;
            else return false; // 异常编码直接返回
            --*bytes;
        }
    } else {
        // 多字节的非首位字节, 应为 10xx xxxx
        if ((c & 0xC0) != 0x80) return false;
        // bytes 来回变化, 最终必须为 0
        --*bytes;
    }    
    return true;
}

//
// isutf8s - 判断字符串是否是utf8编码
// s        : 输入的串
// return   : true 表示 utf8 编码
//
bool 
isutf8s(const char * s) {
    bool ascii = true;
    // bytes 表示编码字节数, utf8 [1, 6] 字节编码
    unsigned char bytes = 0;

    for (unsigned char c; (c = *s); ++s)
        if (!isutf8_local(c, &bytes, &ascii)) 
            return false;

    return !ascii && bytes == 0;
}

//
// isutf8 - check is utf8
// d        : mem
// n        : size
// return   : true 表示 utf8 编码
//
bool 
isutf8(const char d[], size_t n) {
    bool ascii = true;
    // bytes 表示编码字节数, utf8 [1, 6] 字节编码
    unsigned char bytes = 0;

    for (size_t i = 0; i < n; ++i)
        if (!isutf8_local(d[i], &bytes, &ascii)) 
            return false;

    return !ascii && bytes == 0;
}
