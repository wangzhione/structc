#include "json.h"

//
// json_delete - json 对象销毁
// c        : json 对象
// return   : void
//
void 
json_delete(json_t c) {
    while (c) {
        json_t next = c->next;
        unsigned char t = c->type;

        free(c->key);
        if ((t & JSON_STRING) && !(t & JSON_CONST))
            free(c->str);

        // 子结点继续走深度递归删除
        if (c->chid)
            json_delete(c->chid);

        free(c);
        c = next;
    }
}

//
// json_len - 获取 json 对象长度
// c        : json 对象
// return   : 返回 json 对象长度
//
int 
json_len(json_t c) {
    register int len = 0;
    if (c) {
        for (c = c->chid; c; c = c->next)
            ++len;
    }
    return len;
}

//
// json_array - 通过索引获取 json 数组中子结点
// aj       : json 数组
// i        : [0, json_len()) 索引
// return   : 返回对应的数组结点
//
json_t 
json_array(json_t aj, int i) {
    json_t node = aj ? aj->chid : NULL;
    while (node && i > 0) {
        node = node->next;
        --i;
    }
    return node;
}

//
// json_object - 获取 json 对象中子对象 
// obj      : json 对象
// k        : key
// return   : 返回对应的对象结点
//
json_t 
json_object(json_t obj, const char * k) {
    json_t node = obj ? obj->chid : NULL;
    while (node && str_cmpi(k, node->key))
        node = node->next;
    return node;
}

//----------------------------------json parse begin--------------------------------

// parse_number - number 解析
static const char * parse_number(json_t item, const char * str) {
    char c;
    double n = 0;
    int e, eign, sign = 1;

    // 正负号处理判断
    if ((c = *str) == '-' || c == '+') {
        sign = c == '-' ? -1 : 1;
        c = *++str;
    }

    // 整数处理部分
    while (c >= '0' && c <= '9') {
        n = n * 10 + c - '0';
        c = *++str;
    }
    // 处理小数部分
    if (c == '.') {
        int d = 0;
        double s = 1;
        while ((c = *++str) && c >= '0' && c <= '9') {
            d = d * 10 + c - '0';
            s *= 0.1;
        }
        // 得到整数和小数部分
        n += s * d;
    }

    // 添加正负号
    n *= sign;

    // 不是科学计数内容直接返回
    item->type = JSON_NUMBER;
    if (c != 'e' && c != 'E') {
        item->num = n;
        return str;
    }

    // 处理科学计数法
    if ((c = *++str) == '-' || c == '+')
        ++str;
    eign = c == '-' ? -1 : 1;

    e = 0;
    while ((c = *str) >= '0' && c <= '9') {
        e = e * 10 + c - '0';
        ++str;
    }

    // number = +/- number.fraction * 10^+/- exponent
    item->num = n * pow(10, eign * e);
    return str;
}

// parse_literal - 字面串解析
static const char * parse_literal(json_t item, const char * str) {
    char c;
    size_t size;
    const char * etr = '\n' == *str ? ++str : str;

    // 获取到 '`' 字符结尾处
    while ((c = *etr) != '`' && c)
        ++etr;
    if ('`' != c) return NULL;

    // 尝试吃掉 `` 开头第一个和结尾最后一个 \n, 方便整齐划一
    size = '\n' == etr[-1] ? etr - str - 1 : etr - str;

    // 开始构造和填充 json string 结点
    item->type = JSON_STRING;
    item->str = malloc(size + 1);
    memcpy(item->str, str, size);
    item->str[size] = '\0';

    return etr + 1;
}

// parse_hex4 - parse 4 digit hexadecimal number
static unsigned parse_hex4(const char str[]) {
    unsigned h = 0;
    for (unsigned i = 0; ; ++str) {
        unsigned char c = *str;
        if (c >= '0' && c <= '9')
            h += c - '0';
        else if (c >= 'a' && c <= 'f')
            h += c - 'a' + 10;
        else if (c >= 'A' && c <= 'F')
            h += c - 'A' + 10;
        else return 0; // invalid

        // shift left to make place for the next nibble
        if (4 == ++i) break;
        h <<= 4;
    }

    return h;
}

// parse_string - string 解析
static const char * parse_string(json_t item, const char * str) {
    unsigned len = 1;
    char c, * ntr, * out;
    const char * ptr, * etr = str;

    while ((c = *etr) != '"' && c) {
        ++etr;
        // 转义字符特殊处理
        if (c == '\\') {
            if (*etr == '\0') 
                return NULL;
            ++etr;
        }
        ++len;
    }
    if (c != '"') return NULL;

    // 开始复制拷贝内容
    ntr = out = malloc(len);
    for (ptr = str; ptr < etr; ++ptr) {
        // 普通字符直接添加处理
        if ((c = *ptr) != '\\') {
            *ntr++ = c;
            continue;
        }
        // 转义字符处理
        switch ((c = *++ptr)) {
        case 'b': *ntr++ = '\b'; break;
        case 'f': *ntr++ = '\f'; break;
        case 'n': *ntr++ = '\n'; break;
        case 'r': *ntr++ = '\r'; break;
        case 't': *ntr++ = '\t'; break;
        // transcode UTF16 to UTF8. See RFC2781 and RFC3629
        case 'u': {
            // first bytes of UTF8 encoding for a given length in bytes
            static const unsigned char marks[] = { 
                0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC 
            };
            unsigned oc, uc = parse_hex4(ptr + 1);
            // check for invalid
            if ((ptr += 4) >= etr) goto err_free;
            if ((uc >= 0xDC00 && uc <= 0xDFFF) || uc == 0)
                goto err_free;

            // UTF16 surrogate pairs
            if (uc >= 0xD800 && uc <= 0xDBFF) {
                if ((ptr + 6) >= etr) goto err_free;
                // missing second-half of surrogate
                if ((ptr[1] != '\\') || (ptr[2] != 'u' && ptr[2] != 'U')) 
                    goto err_free;

                oc = parse_hex4(ptr + 3);
                ptr += 6; // parse \uXXXX
                // invalid second-half of surrogate
                if (oc < 0xDC00 || oc > 0xDFFF) goto err_free;
                // calculate unicode codepoint from the surrogate pair
                uc = 0x10000 + (((uc & 0x3FF) << 10) | (oc & 0x3FF));
            }

            // encode as UTF8
            // takes at maximum 4 bytes to encode:

            // normal ascii, encoding 0xxxxxxx
            if (uc < 0x80) len = 1;
            // two bytes, encoding 110xxxxx 10xxxxxx
            else if (uc < 0x800) len = 2;
            // three bytes, encoding 1110xxxx 10xxxxxx 10xxxxxx
            else if (uc < 0x10000) len = 3;
            // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            else len = 4;
            ntr += len;

            switch (len) {
            // 10xxxxxx
            case 4: *--ntr = ((uc | 0x80) & 0xBF); uc >>= 6;
            // 10xxxxxx
            case 3: *--ntr = ((uc | 0x80) & 0xBF); uc >>= 6;
            // 10xxxxxx
            case 2: *--ntr = ((uc | 0x80) & 0xBF); uc >>= 6;
            // depending on the length in bytes this determines the 
            // encoding ofthe first UTF8 byte
            case 1: *--ntr = ((uc | marks[len]));
            }
            ntr += len;
        }
        break;
        default : *ntr++ = c;
        }
    }
    *ntr = '\0';
    item->str = out;
    item->type = JSON_STRING;
    return ptr + 1;

err_free:
    free(out);
    return NULL;
}

//
// parse_value - 递归下降解析
// item     : json 结点
// str      : 语句源串
// return   : 解析后剩下的串
//
static const char * parse_value(json_t item, const char * str);

// parse_array - array 解析
static const char * parse_array(json_t item, const char * str) {
    json_t chid;
    item->type = JSON_ARRAY;
    // 空数组直接解析完毕退出
    if (']' == *str) return str + 1;

    // 开始解析数组中数据
    item->chid = chid = json_new();
    str = parse_value(chid, str);
    if (!str) return NULL;

    // array ',' cut
    while (',' == *str) {
        // 支持行尾多一个 ','
        if (']' == *++str)
            return str + 1;

        chid->next = json_new();
        chid = chid->next;
        // 继续间接递归处理值
        str = parse_value(chid, str);
        if (!str) return NULL;
    }

    return ']' == *str ? str + 1 : NULL;
}

// parse_object - object 解析
static const char * parse_object(json_t item, const char * str) {
    json_t chid;
    item->type = JSON_OBJECT;
    if ('}' == *str) return str + 1;
    // "key" check invalid
    if ('"' != *str && *str != '`') return NULL;

    // {"key":value,...} 先处理 key 
    item->chid = chid = json_new();
    if ('"' == *str)
        str = parse_string (chid, str + 1);
    else 
        str = parse_literal(chid, str + 1);

    if (!str || *str != ':') return NULL;
    chid->key = chid->str;
    chid->str = NULL;

    // 再处理 value
    str = parse_value(chid, str + 1);
    if (!str) return NULL;

    // 开始间接递归解析
    while (*str == ',') {
        // 多行解析直接返回结果
        if ('}' == *++str) return str + 1;
        if ('"' != *str && *str != '`') return NULL;

        chid->next = json_new();
        chid = chid->next;
        if ('"' == *str)
            str = parse_string (chid, str + 1);
        else 
            str = parse_literal(chid, str + 1);

        if (!str || *str != ':') return NULL;
        chid->key = chid->str;
        chid->str = NULL;

        str = parse_value(chid, str + 1);
        if (!str) return NULL;
    }

    return '}' == *str ? str + 1 : NULL;
}

static const char * 
parse_value(json_t item, const char * str) {
    if (!str) return NULL;
    switch (*str) {
    // node or N = null, f or F = false, t or T = true ...
    case 'n': case 'N':
        if (str_cmpin(str + 1, "ull", sizeof "ull" - 1)) return NULL;
        item->type = JSON_NULL;
        return str + sizeof "ull"; // exists invalid is you!
    case 't': case 'T':
        if (str_cmpin(str + 1, "rue", sizeof "rue" - 1)) return NULL;
        item->type = JSON_BOOL; item->num = true;
        return str + sizeof "rue";
    case 'f': case 'F':
        if (str_cmpin(str + 1, "alse", sizeof "alse"-1)) return NULL;
        item->type = JSON_BOOL;
        return str + sizeof "alse";
    case '+': case '-': case '.':
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        return parse_number(item, str);
    case '`': return parse_literal(item, str + 1);
    case '"': return parse_string (item, str + 1);
    case '{': return parse_object (item, str + 1);
    case '[': return parse_array  (item, str + 1);
    }
    return NULL;
}

// json_mini - 清洗 str 中冗余的串并返回最终串的长度. 纪念 mini 比男的还平 :)
// EF BB BF     = UTF-8                 (可选标记, 因为 Unicode 标准未有建议)
// FE FF        = UTF-16, big-endian    (大尾字节序标记)
// FF FE        = UTF-16, little-endian (小尾字节序标记, windows Unicode 编码默认标记)
// 00 00 FE FF  = UTF-32, big-endian    (大尾字节序标记)
// FF FE 00 00  = UTF-32, little-endian (小尾字节序标记)
//
size_t json_mini(char * str) {
    char c, * in = str;
    unsigned char * to = (unsigned char *)str;
    
    // 跳过 UTF-8 With BOM 前三个字节
    if (to[0] == 0xEF && to[1] == 0xBB && to[2] == 0xBF)
        to += 3;

    while ((c = *to)) {
        // step 0 : 处理字面串
        if (c == '`') {
            *in++ = c;
            while ((c = *++to) && c != '`')
                *in++ = c;
            if (c) {
                *in++ = c;
                ++to;
            }
            continue;
        }

        // step 1 : 处理字符串
        if (c == '"') {
            *in++ = c;
            while ((c = *++to) && (c != '"' || to[-1] == '\\'))
                *in++ = c;
            if (c) {
                *in++ = c;
                ++to;
            }
            continue;
        }
        // step 2 : 处理不可见特殊字符
        if (c < '!') {
            ++to;
            continue;
        }
        if (c == '/') {
            // step 3 : 处理 // 解析到行末尾
            if (to[1] == '/') {
                while ((c = *++to) && c != '\n')
                    ;
                continue;
            }
            // step 4 : 处理 /*
            if (to[1] == '*') {
                while ((c = *++to) && (c != '*' || to[1] != '/'))
                    ;
                if (c)
                    to += 2;
                continue;
            }
        }
        // step 5 : 合法数据直接保存
        *in++ = *to++;
    }

    *in = '\0';
    return in - str;
}

//
// json_parse - json 解析函数
// str      : json 字符串串
// return   : json 对象, NULL 表示解析失败
//
json_t json_parse(const char * str) {
    json_t c = json_new();
    if (!parse_value(c, str)) {
        json_delete(c);
        return NULL;
    }
    return c;
}

//
// json_file - 通过文件构造 json 对象
// json_create  - 通过字符串构造 json 对象
// str      : 字符串
// path     : 文件路径
// return   : json_t 对象
//
json_t 
json_file(const char * path) {
    // 读取文件中内容, 并检查
    if (!path || !*path) return NULL;
    char * str = str_freads(path);
    if (!str) return NULL;

    // 返回解析结果
    json_t c = json_create(str);
    free(str);
    return c;
}

json_t 
json_create(const char * str) {
    json_t c = NULL;
    if (str && *str) {
        cstr_declare(cs);
        cstr_appends(cs, str);

        // 清洗 + 解析
        json_mini(cs->str);
        c = json_parse(cs->str);

        cstr_free(cs);
    }
    return c;
}

//----------------------------------json parse end----------------------------------

//----------------------------------json print begin--------------------------------

// print_number - number 编码
static char * print_number(json_t item, cstr_t p) {
    char * str;
    double d = item->num;
    
    if (0 == d) {
        str = cstr_expand(p, 2);  // 普通 0 插入
        str[0] = '0'; str[1] = '\0';
    } else {
        int i = (int)d;
        if (fabs(d - i) <= DBL_EPSILON && i <= INT_MAX && i >= INT_MIN) {
            str = cstr_expand(p, 21);
            sprintf(str, "%d", i); // int 值插入
        } else {
            double n = fabs(d);
            str = cstr_expand(p, 64);// e 记数法
            if (fabs(floor(d) - d) <= DBL_EPSILON && n < 1.0e60)
                sprintf(str, "%.0f", d);
            else if (n < 1.0e-6 || n > 1.0e9)
                sprintf(str, "%e", d);
            else 
                sprintf(str, "%f", d);
        }
    }

    return str;
}

// print_string - string 编码
static char * print_string(char * str, cstr_t p) {
    unsigned char c;
    const char * ptr;
    char * ntr, * out;
    // 什么都没有 返回 "" empty string
    if (!str || !*str) {
        out = cstr_expand(p, 3);
        out[0] = out[1] = '"'; out[2] = '\0';
        return out;
    }

    // 获取最终字符输出长度
    size_t len = 0;
    for (ptr = str; (c = *ptr); ++ptr) {
        ++len;
        switch (c) {
        case '\b': case '\t': case '\n':
        case '\f': case '\r':
        case '\\': case '"': ++len; break;
        default:
            if (c < 32) {
                // UTF-16 escape sequence uXXXX
                len += 5;
            }
        }
    }

    // 开始分配内存
    ntr = out = cstr_expand(p, len + 3);
    *ntr++ = '"';
    ntr[len+1] = '\0';

    // 没有特殊字符直接返回
    if (len == (size_t)(ptr - str)) {
        memcpy(ntr, str, len);
        goto ret_out;
    }

    // 存在特殊字符挨个处理
    for (ptr = str; (c = *ptr); ++ptr) {
        if (c >= 32 && c != '"' && c != '\\') {
            *ntr++ = c;
            continue;
        }
        *ntr++ = '\\';
        switch(c) {
        case '\b': *ntr++ = 'b'; break;
        case '\t': *ntr++ = 't'; break;
        case '\n': *ntr++ = 'n'; break;
        case '\f': *ntr++ = 'f'; break;
        case '\r': *ntr++ = 'r'; break;
        case '"': case '\\': *ntr++ = c; break;
        // escape and print as unicode codepoint
        default: sprintf(ntr, "u%04x", c); ntr += 5;
        }
    }

ret_out:
    out[len+1] = '"';
    return out;
}

// print_value - value 编码 Predeclare these prototypes
static char * print_value(json_t item, cstr_t p);

// print_array - array 编码
static char * print_array(json_t item, cstr_t p) {
    size_t n = p->len;
    json_t chid = item->chid;
    char * ptr = cstr_expand(p, 1);

    *ptr = '['; ++p->len;

    // 处理子结点
    while (chid) {
        print_value(chid, p);
        if ((chid = chid->next)) {
            ptr = cstr_expand(p, 1);
            *ptr++ = ','; ++p->len;
        }
    }

    ptr = cstr_expand(p, 2);
    *ptr++ = ']'; *ptr = '\0';
    return p->str + n;
}

// print_object - object 编码
static char * print_object(json_t item, cstr_t p) {
    size_t n = p->len;
    json_t chid = item->chid;
    char * ptr = cstr_expand(p, 1);

    *ptr = '{'; ++p->len;

    // 挨个处理子结点
    while (chid) {
        print_string(chid->key, p);
        p->len += strlen(p->str + p->len);

        // 加入一个 ':'
        ptr = cstr_expand(p, 1);
        *ptr++ = ':'; ++p->len;

        // 接续打印值
        print_value(chid, p);

        if ((chid = chid->next)) {
            ptr = cstr_expand(p, 1);
            *ptr++ = ','; ++p->len;
        }
    }

    ptr = cstr_expand(p, 2);
    *ptr++ = '}'; *ptr = '\0';
    return p->str + n;
}

static char * 
print_value(json_t item, cstr_t cs) {
    char * out = NULL;
    switch(item->type) {
    case JSON_NULL  : 
        strcpy(out = cstr_expand(cs, sizeof "null"), "null"); break;
    case JSON_BOOL  :
        if (!!(item->num)) 
            strcpy(out = cstr_expand(cs, sizeof "true") , "true");
        else 
            strcpy(out = cstr_expand(cs, sizeof "false"), "false");
        break;
    case JSON_NUMBER: out = print_number(item, cs); break;
    case JSON_STRING+JSON_CONST:
    case JSON_STRING: out = print_string(item->str, cs); break;
    case JSON_OBJECT: out = print_object(item, cs); break;
    case JSON_ARRAY : out = print_array(item, cs); break;
    }
    cs->len += strlen(cs->str + cs->len);
    return out;
}

//
// json_print - 获取 json 对象的打印字符串
// c        : json_t 对象
// return   : 返回生成的 json 字符串, 需要自行 free
//
char * 
json_print(json_t c) {
    cstr_declare(cs);
    if (!print_value(c, cs)) {
        cstr_free(cs);
        return NULL;
    }
    return realloc(cs->str, cs->len+1);
}

//----------------------------------json print end----------------------------------

//----------------------------------json utils begin--------------------------------

//
// json_create_array - 创建数组类型 json 对象
// t        : 创建对象 JSON define 类型宏
// a        : 原数组对象
// node        : 原数组长度
// return   : 返回创建好的 json 数组
//
json_t 
json_create_array(unsigned char t, const void * a, int n) {
    json_t node = NULL, foll = NULL, aj = NULL;

    // 可以通过模板分拆做优化
    for (int i = 0; i < n; ++i) {
        switch(t) {
        case JSON_NULL  : node = json_new(); break;
        case JSON_BOOL  : node = json_new_bool(a ? ((bool *)a)[i] : false); break;
        case JSON_NUMBER: node = json_new_number(((double *)a)[i]); break;
        case JSON_STRING+JSON_CONST:
        case JSON_STRING: node = json_new_string( ((char **)a)[i]); break;
        default: return NULL;
        }

        if (i) 
            foll->next = node;
        else {
            aj = json_new_array();
            aj->chid = node;
        }
        foll = node;
    }

    return aj;
}

//
// json_detach_xxxxx - 通过索引分离出 json 子对象
// aj       : json_t 数组
// i        : [0, len()) 索引
// obj      : json_t 对象
// k        : key
// return   : 分离出的 json 对象
//
json_t 
json_detach_array(json_t aj, int i) {
    json_t c, p;
    if ((!aj) || !(c = aj->chid))
        return NULL;

    // 开始查找
    for (p = NULL; i > 0 && c; c = c->next) { 
        p = c;
        --i;
    }
    if (c) {
        if (p)
            p->next = c->next;
        else
            aj->next = c->next;
        c->next = NULL;
    }

    return c;
}

json_t 
json_detach_object(json_t obj, const char * k) {
    json_t c, p;
    if ((!obj) || (!k) || (!*k) || !(c = obj->chid))
        return NULL;
    
    for (p = NULL; c && str_cmpi(c->key, k); c = c->next)
        p = c;
    if (c) {
        if (p)
            p->next = c->next;
        else
            obj->next = c->next;
        c->next = NULL;
    }

    return c;
}

//----------------------------------json utils end----------------------------------
