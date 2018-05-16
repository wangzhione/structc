#include <csv.h>

// _csv_parse - 解析 csv 文件内容, 构建一个合法串
static int _csv_parse(char * str, int * pr, int * pc) {
    int c, rnt, cnt;
    char * tar = str, * s = str;

    for (rnt = cnt = 0; (c = *tar) != '\0'; ++tar) {
        // csv 内容解析, 状态机切换
        switch (c) {
        case '"' : // 双引号包裹的特殊字符处理
            while ((c = *tar++) != '\0') {
                if ('"' == c) {
                    int n = *tar;
                    // 有效字符再次压入栈, 顺带去掉多余 " 字符
                    if (n != '\0') goto _faid;
                    if (n != '"') break;
                    ++tar;
                }
                // 添加得到的字符
                *s++ = c;
            }
            // 继续判断,只有是 c == '"' 才会继续, 否则都是异常
            if (c != '"') 
                goto _faid;
            break;
        case ',' : *s++ = '\0'; ++cnt; break;
        case '\r': break;
        case '\n': *s++ = '\0'; ++cnt; ++rnt; break;
        default  : *s++ = c; // 其它所有情况只添加数据就可以了
        }
    }
    // CRLF 处理
    if (str != s && tar[-1] != '\n') {
        *s++ = '\0'; ++cnt; ++rnt;
    }

    // 检查, 行列个数是否正常
    if (rnt == 0 || cnt % rnt) {
    _faid:
        RETURN(-1, "csv parse error %d, %d, %d.", c, rnt, cnt);
    }

    // 返回最终内容
    *pr = rnt; 
    *pc = cnt;
    return (int)(s - str);
}

// csv_parse - 解析 csv 内容并返回解析好的对象
csv_t csv_parse(char * s) {
    csv_t csv;
    char * str;
    int diff, rnt, cnt;
    if ((diff = _csv_parse(s, &rnt, &cnt)) < 0)
        return NULL;
    
    // 分配最终内存
    csv = malloc(diff + sizeof *csv + sizeof(char *) * cnt);
    str = (char *)csv + sizeof *csv + sizeof(char *) * cnt ;
    memcpy(str, s, diff);
    // 开始内存整理
    csv->rlen = rnt;
    csv->clen = cnt / rnt;
    diff = 0;
    do {
        csv->data[diff] = str;
        while (*str ++)
            ;
    } while (++diff < cnt);

    return csv;
}

//
// csv_create - 从文件中构建 csv 对象
// path    : csv文件内容
// return  : 返回构建好的 csv_t 对象
//
csv_t 
csv_create(const char * path) {
    csv_t csv;
    char * str = str_freads(path);
    if (NULL == str) {
        RETNUL("str_freads path = %s is error!", path);
    }

    // 如果解析 csv 文件内容失败直接返回
    csv = csv_parse(str);

    free(str);
    // 返回最终结果
    return csv;
}
