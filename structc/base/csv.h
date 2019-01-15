#ifndef _CSV_H
#define _CSV_H

//
// csv readonly parse
// 1. 相邻字段必须被单个逗号分隔开, CRLF 换行
// 2. 每个被嵌入的双引号字符必须被表示为两个双引号字符
// 3. 字段都可以被双引号包裹, 有回车换行符双引号或者逗号, 那必须包裹
//

#include "struct.h"
#include "strext.h"

typedef struct {   // struct in heap malloc
    int    rlen;   // 数据行数, 索引 [0, rlen)
    int    clen;   // 数据列数, 索引 [0, clen)
    char * data[]; // 保存数据, rlen * clen '二维数组
} * csv_t;

//
// csv_get - 获取 csv[r][c] 位置的字符串
// csv     : csv_t 对象
// r       : 行索引 [0, csv->rlen)
// c       : 列索引 [0, csv->clen)
// return  : 返回 csv[r][c], 后面可以用 atoi, atof, strdup ...
//
inline const char * csv_get(csv_t csv, int r, int c) {
    DCODE({
        if (!csv || r < 0 || r >= csv->rlen || c < 0 || c >= csv->clen) {
            RETNUL("params is error csv:%p, r:%d, c:%d.", csv, r, c);
        }
    });

    // 返回 csv[r][c] 索引位置字符串
    return csv->data[r * csv->clen + c];
}

//
// csv_delete - 释放 csv_t 对象
// csv     : csv_t 对象
// return  : void 
//
inline void csv_delete(csv_t csv) {
    free(csv);
}

//
// csv_create - 文件中构建 csv_t 对象
// path    : 文件路径
// return  : 返回 csv_t 对象, NULL is error
//
extern csv_t csv_create(const char * path);

#endif//_CSV_H
