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

        free(c->keys);
        if ((t & JSON_STRING) && !(t & JSON_CONST))
            free(c->vals);

        // 子结点 继续递归删除
        if (c->chid)
            json_delete(c->chid);

        c = next;
    }
}

//
// json_len - 获取当前 json 数组长度
// arr      : 待处理的 json_t 数组
// return   : 返回对象中长度
//
int 
json_len(json_t arr) {
    int len = 0;
    if (arr) {
        for (arr = arr->chid; arr; arr = arr->next)
            ++len;
    }
    return len;
}

//
// json_array - 获取 array 数组中子结点
// arr      : 待处理的 json_t 数组
// i        : [0, len(array)) 索引
// return   : 返回查询到数组结点
//
json_t 
json_array(json_t arr, int i) {
    json_t n = arr->chid;
    while (n && i > 0) {
        --i;
        n = n->next;
    }
    return n;
}

//
// json_object - 获取 object 对象中子对象 
// obj      : 待处理的 json_t 对象
// k        : 待查询 key
// return   : 返回查询到对象结点
//
json_t 
json_object(json_t obj, const char * k) {
    json_t n = obj->chid;
    while (n && str_cmpi(k, n->keys))
        n = n->next;
    return n;
}

//-------------------------------------json parse begin-----------------------------------

// number 分析
static const char * parse_number(json_t item, const char * str) {
    char c;
    int sign = 1;
    double n = 0;
    int e, esign;

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
        item->vald = n;
        return str;
    }

    // 处理科学计数法
    if ((c = *++str) == '-' || c == '+')
        ++str;
    esign = c == '-' ? -1 : 1;

    e = 0;
    while ((c = *str) >= '0' && c <= '9') {
        e = e * 10 + c - '0';
        ++str;
    }

    // number = +/- number.fraction * 10^+/- exponent
    item->vald = n * pow(10, esign * e);
    return str;
}

// parse 4 digit hexadecimal number
static unsigned parse_hex4(const char str[]) {
    unsigned h, i;
    unsigned char c;
    for (h = i = 0; ; ++str) {
        c = *str;
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

// string 分析
static const char * parse_string(json_t item, const char * str) {
    const char * ptr, * etr = str;
    char c, * ntr, * out;
    unsigned len = 1;

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
            static const unsigned char marks[] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
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
            // depending on the length in bytes this determines the encoding ofthe first UTF8 byte
            case 1: *--ntr = ((uc | marks[len]));
            }
            ntr += len;
            break;
        }
        default : *ntr++ = c;
        }
    }
    *ntr = '\0';
    item->vals = out;
    item->type = JSON_STRING;
    return ptr + 1;

err_free:
    free(out);
    return NULL;
}

//
// parse_value - 递归下降解析
// item     : 待填充的 json 结点
// str      : 语句源串
// return   : 解析后串
//
static const char * parse_value(json_t item, const char * str);

// array 解析
static const char * parse_array(json_t item, const char * str) {
    json_t chid;
    item->type = JSON_ARRAY;
    // 空数组直接解析完毕退出
    if (']' == *str) return str + 1;

    // 开始解析数组中数据
    item->chid = chid = json_new();
    str = parse_value(chid, str);
    if (NULL == str) return NULL;

    // array ',' cut
    while (',' == *str) {
        // 支持行尾多一个 ','
        if (']' == *++str)
            return str + 1;

        chid->next = json_new();
        chid = chid->next;
        // 继续间接递归处理值
        str = parse_value(chid, str);
        if (NULL == str) return NULL;
    }

    return ']' == *str ? str + 1 : NULL;
}

// object 解析
static const char * parse_object(json_t item, const char * str) {
    json_t chid;
    item->type = JSON_OBJECT;
    if ('}' == *str) return str + 1;
    // "key" check invalid
    if ('"' != *str) return NULL;

    // {"key":value,...} 先处理 key 
    item->chid = chid = json_new();
    str = parse_string(chid, str + 1);
    if (!str || *str != ':') return NULL;
    chid->keys = chid->vals;
    chid->vals = NULL;

    // 再处理 value
    str = parse_value(chid, str + 1);
    if (NULL == str) return NULL;

    // 开始间接递归解析
    while (*str == ',') {
        // 多行解析直接返回结果
        if ('}' == *++str) return str + 1;
        if ('"' != *str) return NULL;

        chid->next = json_new();
        str = parse_string(chid = chid->next, str + 1);
        if (!str || *str != ':') return NULL;
        chid->keys = chid->vals;
        chid->vals = NULL;

        str = parse_value(chid, str + 1);
        if (NULL == str) return NULL;
    }

    return '}' == *str ? str + 1 : NULL;
}

static const char * 
parse_value(json_t item, const char * str) {
    char c;
    if ((!str) || !(c = *str)) return NULL;

    switch (c) {
    // n or N = null, f or F = false, t or T = true ...
    case 'n': case 'N':
        if (str_cmpin(str + 1, "ull", sizeof "ull" - 1)) return NULL;
        item->type = JSON_NULL;
        return str + 4; // exists invalid is you!
    case 't': case 'T':
        if (str_cmpin(str + 1, "rue", sizeof "rue" - 1)) return NULL;
        item->type = JSON_TRUE; item->vald = true;
        return str + 4;
    case 'f': case 'F':
        if (str_cmpin(str + 1, "alse", sizeof "alse" - 1)) return NULL;
        item->type = JSON_FALSE;
        return str + 5;
    case '+': case '-': case '.':  
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        return parse_number(item, str);
    case '"': return parse_string(item, str + 1);
    case '{': return parse_object(item, str + 1);
    case '[': return parse_array(item, str + 1);
    }
    return NULL;
}

//  将 str 中不需要解析串都去掉, 返回最终串的长度. 并且纪念 mini 比男的还平
size_t json_mini(char * str) {
    char c, * in = str, * to = str;
    
    while ((c = *to)) {
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
// str      : json char * 串
// return   : json 对象, NULL 表示解析失败
//
json_t json_parse(const char * str) {
    json_t c = json_new();
    if (!parse_value(c, str)) {
        json_delete(c);
        c = NULL;
    }
    return c;
}

//
// json_file - 通过 path   路径构造 json 对象
// json_create  - 通过 char * 对象构造 json 对象
// str      : const char * 字符串
// path     : 文件路径
// return   : json_t 对象
//
json_t 
json_file(const char * path) {
    json_t c;
    char * str;
    // 读取文件中内容, 并检查
    if (!path || !*path) return NULL;
    str = str_freads(path);
    if (!str) return NULL;

    // 返回解析结果
    c = json_create(str);
    free(str);
    return c;
}

json_t 
json_create(const char * str) {
    json_t c = NULL;
    if (str && *str) {
        TSTR_CREATE(tsr);
        tstr_appends(tsr, str);

        // 清洗 + 解析
        json_mini(tsr->str);
        c = json_parse(tsr->str);

        TSTR_DELETE(tsr);
    }
    return c;
}

//-------------------------------------json parse end-------------------------------------

//-------------------------------------json print begin-----------------------------------

// number 打印
static char * print_number(json_t item, tstr_t p) {
    char * str;
    double d = item->vald;
    
    if (0 == d) {
        str = tstr_expand(p, 2);  // 普通 0 插入
        str[0] = '0'; str[1] = '\0';
    } else {
        int i = (int)d;
        if (fabs(d - i) <= DBL_EPSILON && i <= INT_MAX && i >= INT_MIN) {
            str = tstr_expand(p, 21);
            sprintf(str, "%d", i); // int 值插入
        } else {
            double n = fabs(d);
            str = tstr_expand(p, 64);// e 记数法
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

// string 打印
static char * print_string(char * str, tstr_t p) {
    size_t len;
    unsigned char c;
    const char * ptr;
    char * ntr, * out;
    // 什么都没有 返回 "" empty string
    if (!str || !*str) {
        out = tstr_expand(p, 3);
        out[0] = out[1] = '"'; out[2] = '\0';
        return out;
    }

    // 获取最终字符输出长度
    len = 0;
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
    ntr = out = tstr_expand(p, len + 3);
    *ntr++ = '"';
    ntr[len+1] = '\0';

    // 没有特殊字符直接返回
    if (len == ptr - str) {
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

// value 打印 Predeclare these prototypes
static char * print_value(json_t item, tstr_t p);

// array 打印
static char * print_array(json_t item, tstr_t p) {
    size_t n = p->len;
    json_t chid = item->chid;
    char * ptr = tstr_expand(p, 1);

    *ptr = '['; ++p->len;

    // 处理子结点
    while (chid) {
        print_value(chid, p);
        if ((chid = chid->next)) {
            ptr = tstr_expand(p, 1);
            *ptr++ = ','; ++p->len;
        }
    }

    ptr = tstr_expand(p, 2);
    *ptr++ = ']'; *ptr = '\0';
    return p->str + n;
}

// object 打印
static char * print_object(json_t item, tstr_t p) {
    size_t n = p->len;
    json_t chid = item->chid;
    char * ptr = tstr_expand(p, 1);

    *ptr = '{'; ++p->len;

    // 挨个处理子结点
    while (chid) {
        print_string(chid->keys, p);
        p->len += strlen(p->str + p->len);

        // 加入一个 ':'
        ptr = tstr_expand(p, 1);
        *ptr++ = ':'; ++p->len;

        // 接续打印值
        print_value(chid, p);

        if ((chid = chid->next)) {
            ptr = tstr_expand(p, 1);
            *ptr++ = ','; ++p->len;
        }
    }

    ptr = tstr_expand(p, 2);
    *ptr++ = '}'; *ptr = '\0';
    return p->str + n;
}

static char * 
print_value(json_t item, tstr_t p) {
    char * out = NULL;
    switch(item->type) {
    case JSON_NULL  : out = tstr_expand(p, 5); strcpy(out, "null"); break;
    case JSON_TRUE  : out = tstr_expand(p, 5); strcpy(out, "true"); break;
    case JSON_FALSE : out = tstr_expand(p, 6); strcpy(out, "false"); break;
    case JSON_NUMBER: out = print_number(item, p); break;
    case JSON_STRING+JSON_CONST:
    case JSON_STRING: out = print_string(item->vals, p); break;
    case JSON_OBJECT: out = print_object(item, p); break;
    case JSON_ARRAY : out = print_array(item, p); break;
    }
    p->len += strlen(p->str + p->len);
    return out;
}

//
// json_print - 获取 json 对象的打印字符串
// c        : json_t 对象
// return   : 返回生成的 json 字符串, 需要自行 free
//
char * 
json_print(json_t c) {
    TSTR_CREATE(str);
    if (NULL == print_value(c, str)) {
        TSTR_DELETE(str);
        return NULL;
    }
    return realloc(str->str, str->len + 1);
}

//-------------------------------------json print end-------------------------------------

//-------------------------------------json utils begin-----------------------------------

//
// json_new_arrays - 通过类型创建类型数组 json 对象
// t        : 类型宏
// arr      : 源数组对象
// n        : 源数组长度
// return   : 返回创建好的数组 json 对象
//
json_t 
json_new_arrays(unsigned char t, const void * arr, int n) {
    int i = 0;
    json_t m, p, a;

    for (m = p = a = NULL; i < n; ++i) {
        switch(t) {
        case JSON_NULL  : m = json_new_null(); break;
        case JSON_TRUE  : m = json_new_bool(arr ? ((bool *)arr)[i] : true); break;
        case JSON_FALSE : m = json_new_bool(arr ? ((bool *)arr)[i] :false); break;
        case JSON_NUMBER: m = json_new_number(((double *)arr)[i]); break;
        case JSON_STRING+JSON_CONST:
        case JSON_STRING: m = json_new_string(((char **)arr)[i]); break;
        default: return NULL;
        }

        p = m;
        if (i) p->next = m;
        else {
            a = json_new_array();
            a->chid = m;
        }
    }

    return a;
}

//
// json_detach_xxxxx - 通过索引分离出 json 子 json 对象
// arr      : 待处理的 json_t 数组
// i        : [0, len(array)) 索引
// obj      : 待处理的 json_t 对象
// k        : 待查询 key
// return   : 分离后的对象
//
json_t 
json_detach_array(json_t arr, int i) {
    json_t c, p;
    if ((!arr) || !(c = arr->chid))
        return NULL;

    // 开始查找
    for (p = NULL; i > 0 && c; c = c->next) { 
        p = c;
        --i;
    }
    if (NULL != c) {
        if (NULL != p)
            arr->chid = c->next;
        else
            p->next = c->next;
        c->next = NULL;
    }

    return c;
}

json_t 
json_detach_object(json_t obj, const char * k) {
    json_t c, p;
    if ((!obj) || (!k) || (!*k) || !(c = obj->chid))
        return NULL;
    
    for (p = NULL; c && str_cmpi(c->keys, k); c = c->next)
        p = c;
    if (NULL != c) {
        if (NULL != p)
            obj->chid = c->next;
        else
            p->next = c->next;
        c->next = NULL;
    }

    return c;
}

//-------------------------------------json utils end-------------------------------------
