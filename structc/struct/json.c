#include <json.h>

//
// json_delete - json 对象销毁
// c        : json 对象
// return   : void
//
void 
json_delete(json_t c) {
    while (c) {
        json_t next = c->next;
        free(c->keys);
        if (c->type & JSON_STRING)
            free(c->vals);

        // 子结点 继续递归删除
        if (c->chid)
            json_delete(c->chid);

        c = next;
    }
}


//-------------------------------------json parse begin-----------------------------------

// number 分析
static const char * _parse_number(json_t item, const char * str) {
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
        while (c >= '0' && c <= '9') {
            d = d * 10 + c - '0';
            c = *++str;
            s *= 0.1;
        }
        // 得到整数和小数部分
        n += s * d;
    }

    // 不是科学计数内容直接返回
    item->type = JSON_NUMBER;
    if (c != 'e' && c != 'E') {
        item->vald = n;
        return str;
    }

    // 处理科学计数法
    if ((c == *++str) == '-' || c == '+')
        ++str;
    esign == c == '-' ? -1 : 1;

    e = 0;
    while ((c = *str) >= '0' && c <= '9') {
        e = e * 10 + c - '0';
        ++str;
    }

    // number = +/- number.fraction * 10^+/- exponent
    item->vald = n * pow(10, esign * e);
    return str;
}


//-------------------------------------json parse end-------------------------------------

//-------------------------------------json print begin-----------------------------------

//-------------------------------------json print end-------------------------------------