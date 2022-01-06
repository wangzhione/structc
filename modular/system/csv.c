#include "csv.h"

// csv_check - 解析和检查 csv 文件内容, 返回构造的合法串长度
static int csv_check(char * str, int * pr, int * pc) {
    int c, rnt = 0, cnt = 0;
    char * tar = str, * s = str;
    while ((c = *tar++) != '\0') {
        // csv 内容解析, 状态机切换
        switch (c) {
        case '"' : // 双引号包裹的特殊字符处理
            while ((c = *tar++) != '\0') {
                if ('"' == c) {
                    // 有效字符再次压入栈, 顺带去掉多余 " 字符
                    if (*tar != '"') 
                        break;
                    ++tar;
                }
                // 添加得到的字符
                *s++ = c;
            }
            // 继续判断,只有是 c == '"' 才会继续, 否则都是异常
            if (c != '"') 
                goto err_faid;
            break;
        case ',' : *s++ = '\0'; ++cnt; break;
        case '\r': break;
        case '\n': *s++ = '\0'; ++cnt; ++rnt; break;
        default  : *s++ = c; // 其它所有情况只添加数据就可以了
        }
    }
    // CRLF 处理
    if (str != s && (c = tar[-2]) && c != '\n') {
        *s++ = '\0'; ++cnt; ++rnt;
    }

    // 检查, 行列个数是否正常
    if (rnt == 0 || cnt % rnt) {
err_faid:
        RETURN(-1, "csv parse error %d, %d, %d.", c, rnt, cnt);
    }

    // 返回最终内容
    *pr = rnt; *pc = cnt;
    return (int)(s - str);
}

// csv_parse - 解析字节流返回 csv 对象
csv_t csv_parse(char * str) {
    int n, rnt, cnt;
    if ((n = csv_check(str, &rnt, &cnt)) < 0)
        return NULL;
    
    // 分配最终内存
    csv_t csv = malloc(n + sizeof *csv + sizeof(char *) * cnt);
    char * s = (char *)csv + sizeof *csv + sizeof(char *) * cnt;
    memcpy(s, str, n);

    // 开始内存整理, csv 字段填充
    n = 0;
    csv->rlen = rnt;
    csv->clen = cnt / rnt;
    do {
        csv->data[n] = s;
        while (*s++)
            ;
    } while (++n < cnt);

    return csv;
}

//
// csv_create - 文件中构建 csv_t 对象
// path    : 文件路径
// return  : 返回 csv_t 对象, NULL is error
//
csv_t 
csv_create(const char * path) {
    char * str = str_freads(path);
    if (str) {
        // 开始解析 csv 文件内容, 并返回最终结果
        csv_t csv = csv_parse(str);
        free(str);
        return csv;   
    }
    // 意外返回 NULL
    RETNUL("str_freads path = %s is error!", path);
}
