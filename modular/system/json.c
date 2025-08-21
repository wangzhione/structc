#include "json.h"
#include "q.h"

void json_delete_recursion(json_t cj) {
    while (cj) {
        json_t next = cj->next;
        unsigned type = cj->type;

        free(cj->key);
        if ((type & JSON_STRING) && !(type & JSON_CONST))
            free(cj->str);

        // 子结点继续走深度递归删除
        if (cj->child)
            json_delete(cj->child);

        free(cj);
        cj = next;
    }
}

void json_delete(json_t cj) {
    if (cj == nullptr) {
        return;
    }

    struct q q;
    if (q_init(&q) == false) {
        // 把崩溃从堆推向了栈
        return json_delete_recursion(cj);
    }

    do {
        json_t next = cj->next;
        unsigned type = cj->type;
        
        free(cj->key);
        if ((type & JSON_STRING) && !(type & JSON_CONST))
            free(cj->str);

        if (cj->child) {
            if (q_push(&q, cj->child) == false) {
                // 堆上以及没有内存, 这时候尝试崩溃, 当然这里纯为了学习, 而无中生有
                EXIT("q_push panic memory");
            }
        }

        free(cj);
        cj = next ? next : q_pop(&q);
    } while (cj);

    q_release(&q);
}

//
// json_len - 获取 json 对象长度
// c        : json 对象
// return   : 返回 json 对象长度
//
int 
json_len(json_t cj) {
    register int len = 0;
    if (cj) {
        for (cj = cj->child; cj; cj = cj->next)
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
    if (aj == nullptr || i <= 0) return nullptr;

    json_t node = aj->child;
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
    if (obj == nullptr || k == nullptr) return nullptr;

    json_t node = obj->child;
    while (node && strcasecmp(k, node->key))
        node = node->next;
    return node;
}

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
    if ('`' != c) return nullptr;

    // 尝试吃掉 `` 开头第一个和结尾最后一个 \n, 方便整齐划一
    size = '\n' == etr[-1] ? etr - str - 1 : etr - str;

    // 开始构造和填充 json string 结点
    item->type = JSON_STRING;
    item->str = malloc(size + 1);
    assert(item->str != nullptr);
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
        if (++i == 4) break;
        h <<= 4;
    }

    return h;
}

// parse_string - string 解析
static const char * parse_string(json_t item, const char * str) {
    unsigned len = 1;
    char c, * cursor, * out;
    const char * ptr, * etr = str;

    while ((c = *etr) != '"' && c) {
        ++etr;
        // 转义字符特殊处理
        if (c == '\\') {
            if (*etr == '\0') 
                return nullptr;
            ++etr;
        }
        ++len;
    }
    if (c != '"') return nullptr;

    // 开始复制拷贝内容
    cursor = out = malloc(len);
    assert(out != nullptr);
    for (ptr = str; ptr < etr; ++ptr) {
        // 普通字符直接添加处理
        if ((c = *ptr) != '\\') {
            *cursor++ = c;
            continue;
        }
        // 转义字符处理
        switch ((c = *++ptr)) {
        case 'b': *cursor++ = '\b'; break;
        case 'f': *cursor++ = '\f'; break;
        case 'n': *cursor++ = '\n'; break;
        case 'r': *cursor++ = '\r'; break;
        case 't': *cursor++ = '\t'; break;
        // transcode UTF16 to UTF8. See RFC2781 and RFC3629
        case 'u': {
            // first bytes of UTF8 encoding for a given length in bytes
            static const unsigned char marks[] = { 
                0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC 
            };
            unsigned oc, uc = parse_hex4(ptr + 1);
            // check for invalid
            if ((ptr += 4) >= etr) goto faild_free;
            if ((uc >= 0xDC00 && uc <= 0xDFFF) || uc == 0)
                goto faild_free;

            // UTF16 surrogate pairs
            if (uc >= 0xD800 && uc <= 0xDBFF) {
                if ((ptr + 6) >= etr) goto faild_free;
                // missing second-half of surrogate
                if ((ptr[1] != '\\') || (ptr[2] != 'u' && ptr[2] != 'U')) 
                    goto faild_free;

                oc = parse_hex4(ptr + 3);
                ptr += 6; // parse \uXXXX
                // invalid second-half of surrogate
                if (oc < 0xDC00 || oc > 0xDFFF) goto faild_free;
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
            cursor += len;

            switch (len) {
            // 10xxxxxx
            case 4: *--cursor = ((uc | 0x80) & 0xBF); uc >>= 6; __attribute__((fallthrough));
            // 10xxxxxx
            case 3: *--cursor = ((uc | 0x80) & 0xBF); uc >>= 6; __attribute__((fallthrough));
            // 10xxxxxx
            case 2: *--cursor = ((uc | 0x80) & 0xBF); uc >>= 6; __attribute__((fallthrough));
            // depending on the length in bytes this determines the 
            // encoding ofthe first UTF8 byte
            case 1: *--cursor = ((uc | marks[len]));
            }
            cursor += len;
            break;
        }
        default : *cursor++ = c;
        }
    }
    *cursor = '\0';
    item->str = out;
    item->type = JSON_STRING;
    return ptr + 1;

faild_free:
    free(out);
    return nullptr;
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
    json_t child;
    item->type = JSON_ARRAY;
    // 空数组直接解析完毕退出
    if (']' == *str) return str + 1;

    // 开始解析数组中数据
    item->child = child = json_new();
    str = parse_value(child, str);
    if (!str) return nullptr;
    item->len++;

    // array ',' cut
    while (',' == *str) {
        // 支持行尾多一个 ','
        if (']' == *++str)
            return str + 1;

        child->next = json_new();
        child = child->next;
        // 继续间接递归处理值
        str = parse_value(child, str);
        if (!str) return nullptr;
            item->len++;
    }

    return ']' == *str ? str + 1 : nullptr;
}

// parse_object - object 解析
static const char * parse_object(json_t item, const char * str) {
    json_t child;
    item->type = JSON_OBJECT;
    if ('}' == *str) return str + 1;
    // "key" check invalid
    if ('"' != *str && *str != '`') return nullptr;

    // {"key":value,...} 先处理 key 
    item->child = child = json_new();
    if ('"' == *str)
        str = parse_string (child, str + 1);
    else 
        str = parse_literal(child, str + 1);

    if (!str || *str != ':') return nullptr;
    child->key = child->str;
    child->str = nullptr;

    // 再处理 value
    str = parse_value(child, str + 1);
    if (!str) return nullptr;
    item->len++;

    // 开始间接递归解析
    while (*str == ',') {
        // 多行解析直接返回结果
        if ('}' == *++str) return str + 1;
        if ('"' != *str && *str != '`') return nullptr;

        child->next = json_new();
        child = child->next;
        if ('"' == *str)
            str = parse_string (child, str + 1);
        else 
            str = parse_literal(child, str + 1);

        if (!str || *str != ':') return nullptr;
        child->key = child->str;
        child->str = nullptr;

        str = parse_value(child, str + 1);
        if (!str) return nullptr;
        item->len++;
    }

    return '}' == *str ? str + 1 : nullptr;
}

static const char * parse_value(json_t item, const char * str) {
    if (!str) return nullptr;
    switch (*str) {
    // n or N = nullptr, f or F = false, t or T = true
    case 'n': case 'N':
        if (strncasecmp(str + 1, "ull", sizeof "ull" - 1)) return nullptr;
        item->type = JSON_nullptr;
        return str + sizeof "ull"; // exists invalid is you!
    case 't': case 'T':
        if (strncasecmp(str + 1, "rue", sizeof "rue" - 1)) return nullptr;
        item->type = JSON_TRUE; item->num = true;
        return str + sizeof "rue";
    case 'f': case 'F':
        if (strncasecmp(str + 1, "alse", sizeof "alse"-1)) return nullptr;
        item->type = JSON_FALSE;
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
    return nullptr;
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
// return   : json 对象, nullptr 表示解析失败
//
json_t json_parse(const char * str) {
    json_t cj = json_new();
    if (parse_value(cj, str) == nullptr) {
        json_delete(cj);
        return nullptr;
    }
    return cj;
}

json_t 
json_create(const char * str) {
    if (str == nullptr || *str == 0) 
        return nullptr;

    char * ss = strdup(str);
    // 清洗 + 解析
    json_mini(ss);
    json_t c = json_parse(ss);
    free(ss);
    return c;
}

json_t 
json_file(const char * path) {
    char * str = str_freads(path);
    // 读取文件中内容, 并事先检查参数
    if (str == nullptr)
        return nullptr;

    // 尝试解析结果
    json_t c = json_create(str);
    free(str);
    return c;
}

json_t 
json_detach_array(json_t aj, int i) {
    if (aj == nullptr) return nullptr;

    json_t p = nullptr;
    json_t c = aj->child;

    // 开始查找
    while (i > 0 && c) {
        i--;
        p = c;
        c = c->next;
    }
    
    // 潜规则, i >= len 时候获取的是 i = len-1 处索引
    if (c) {
        (p ? p : aj)->next = c->next;
        c->next = nullptr;
    }
    return c;
}

json_t 
json_detach_object(json_t obj, const char * k) {
    if (obj == nullptr || k == nullptr || *k == 0) return nullptr;

    json_t p = nullptr;
    json_t c = obj->child;
    
    while (c && strcasecmp(c->key, k)) {
        p = c;
        c = c->next;
    }
    if (c) {
        (p ? p : obj)->next = c->next;
        c->next = nullptr;
    }

    return c;
}

// json_string_number - number 编码
static char * json_string_number(json_t item, struct chars * p) {
    char * str;
    double d = item->num;
    
    // 普通 0 插入 
    if (d >= -DBL_EPSILON && d <= DBL_EPSILON) {
        str = chars_expand(p, 2);
        str[0] = '0'; str[1] = 0;
        p->len += 1;
        return str;
    }

    int i = (int)d;
    if (fabs(d - i) <= DBL_EPSILON && i <= INT_MAX && i >= INT_MIN) {
        str = chars_expand(p, 21);
        sprintf(str, "%d", i); // int 值插入
    } else {
        double n = fabs(d);
        str = chars_expand(p, 64); // e 科学记数法
        if (fabs(d - floor(d)) <= DBL_EPSILON && n < 1.0e60)
            sprintf(str, "%.0f", d);
        else if (n < 1.0e-6 || n > 1.0e9)
            sprintf(str, "%e", d);
        else 
            sprintf(str, "%f", d);
    }
    p->len += strlen(str);

    return str;
}

// json_string_string - string 编码
static char * json_string_string(char * str, struct chars * p) {
    unsigned char c;
    const char * ptr;
    char * cursor, * out;
    // 什么都没有 返回 "" empty string
    if (nullptr == str || *str == 0) {
        out = chars_expand(p, 3);
        out[0] = out[1] = '"'; out[2] = '\0';
        p->len += 2;
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
    cursor = out = chars_expand(p, len+3);
    out[len+2] = 0;
    *cursor++ = '"';

    // 没有特殊字符直接返回
    if (len == (size_t)(ptr - str)) {
        memcpy(cursor, str, len);
        goto ret_out;
    }

    // 存在特殊字符挨个处理
    for (ptr = str; (c = *ptr); ++ptr) {
        if (c >= 32 && c != '"' && c != '\\') {
            *cursor++ = c;
            continue;
        }
        *cursor++ = '\\';
        switch(c) {
        case '\b': *cursor++ = 'b'; break;
        case '\t': *cursor++ = 't'; break;
        case '\n': *cursor++ = 'n'; break;
        case '\f': *cursor++ = 'f'; break;
        case '\r': *cursor++ = 'r'; break;
        case '"': case '\\': *cursor++ = c; break;
        // escape and print as unicode codepoint
        default: sprintf(cursor, "u%04x", c); cursor += 5;
        }
    }

ret_out:
    out[len+1] = '"';
    p->len += len+2;
    return out;
}

// json_string_value - value 编码 Predeclare these prototypes
static char * json_string_value(json_t item, struct chars * p);

// json_string_array - array 编码
static char * json_string_array(json_t item, struct chars * p) {
    size_t n = p->len;
    json_t child = item->child;
    char * ptr = chars_expand(p, 1);

    *ptr = '['; ++p->len;

    // 处理子结点
    while (child) {
        json_string_value(child, p);
        if ((child = child->next)) {
            ptr = chars_expand(p, 1);
            *ptr++ = ','; ++p->len;
        }
    }

    ptr = chars_expand(p, 2);
    *ptr++ = ']'; ++p->len; 
    *ptr = '\0';
    return p->str + n;
}

// json_string_object - object 编码
static char * json_string_object(json_t item, struct chars * p) {
    size_t n = p->len;
    json_t child = item->child;
    char * ptr = chars_expand(p, 1);

    *ptr = '{'; ++p->len;

    // 挨个处理子结点
    while (child) {
        json_string_string(child->key, p);
        p->len += strlen(p->str + p->len);

        // 加入一个 ':'
        ptr = chars_expand(p, 1);
        *ptr++ = ':'; ++p->len;

        // 接续打印值
        json_string_value(child, p);

        if ((child = child->next)) {
            ptr = chars_expand(p, 1);
            *ptr++ = ','; ++p->len;
        }
    }

    ptr = chars_expand(p, 2);
    *ptr++ = '}'; ++p->len;
    *ptr = '\0';
    return p->str + n;
}

static char * 
json_string_value(json_t item, struct chars * p) {
    char * out = nullptr;
    switch(item->type) {
    case JSON_nullptr  :
        out = memcpy(chars_expand(p, sizeof "nullptr"), "nullptr", sizeof "nullptr");
        p->len += sizeof "nullptr" - 1;
        return out;
    case JSON_TRUE  :
        out = memcpy(chars_expand(p, sizeof "true"), "true", sizeof "true");
        p->len += sizeof "true" - 1;
        return out;
    case JSON_FALSE :
        out = memcpy(chars_expand(p, sizeof "false"), "false", sizeof "false");
        p->len += sizeof "false" - 1;
        return out;

    case JSON_NUMBER: return json_string_number(item, p);
    case JSON_STRING+JSON_CONST:
    case JSON_STRING: return json_string_string(item->str, p);
    case JSON_OBJECT: return json_string_object(item, p);
    case JSON_ARRAY : return json_string_array(item, p);
    // default all right, lack of defense
    }
    return out;
}

//
// json_string - 获取 json 对象的打印字符串
// cj       : json_t 对象
// return   : 返回生成的 json 字符串, 需要自行 free
//
char * 
json_string(json_t cj) {
    struct chars p = {};
    if (nullptr == json_string_value(cj, &p)) {
        free(p.str);
        return nullptr;
    }
    return p.str;
}

