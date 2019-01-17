#include "url.h"

//
// url_encode - url 编码, 需要自己 free
// s        : url串
// len      : url串长度
// nen      : 返回编码后串长度
// return   : 返回编码后串的首地址
// 
char * 
url_encode(const char * s, int len, int * nen) {
    static const unsigned char encode[] = "0123456789ABCDEF";

    if (!s || !*s || len <= 0) {
        if (nen) *nen = 0;
        return NULL;
    }

    const unsigned char * from = (unsigned char *)s;
    const unsigned char * end = (unsigned char *)s + len;
    unsigned char * to = calloc(3 * len + 1, sizeof(unsigned char));
    unsigned char * start = to;

    while (from < end) {
        register unsigned char c = *from++;
        if (c == ' ') {
            *to++ = '+';
            continue;
        }

        // [a-z] [A-Z] [0-9] [&-./:=?_] 以外字符采用二进制替代
        if ((c < '0' && c != '&' && c != '-' && c != '.' && c != '/') ||
            (c < 'A' && c >  '9' && c != ':' && c != '=' && c != '?') ||
            (c > 'Z' && c  < 'a' && c != '_') ||
            (c > 'z')) {
            to[0] = '%';
            to[1] = encode[c >> 4];
            to[2] = encode[c & 15];
            to += 3;
            continue;
        }

        *to++ = c;
    }
    *to = '\0';

    // 返回结果
    if (nen) *nen = (int)(to - start);
    return (char *)start;
}

// htoc - 2字节变成16进制数表示
inline char htoc(char * s) {
    int v, c= s[0];
    // 小写变大写是兼容性写法
    if (islower(c)) c = toupper(c);
    v =  (c >= '0' && c <= '9' ? c - '0' : c - 'A' + 10) * 16;

    c = s[1];
    if (islower(c)) c = toupper(c);
    v += (c >= '0' && c <= '9' ? c - '0' : c - 'A' + 10);

    return (char)v;
}

//
// url_decode - url 解码, 解码后也是放在 s[] 中
// s        : 待解码的串
// len      : 解码串长度
// return   : 返回解码串的长度, < 0 表示失败
//
int 
url_decode(char s[], int len) {
    if (!s || !*s || len <= 0)
        return -1;

    char * dest = s,  * data = s;
    while (len--) {
        char c = *data++;
        // 反向解码
        if (c == '+')
            *dest = ' ';
        else if (c == '%' && len >= 2 
                          && isxdigit(data[0]) 
                          && isxdigit(data[1])) {
            *dest = htoc(data);
            data += 2;
            len -= 2;
        }
        else
            *dest = c;
        ++dest;
    }
    *dest = '\0';

    return (int)(dest - s);
}
