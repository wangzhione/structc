#ifndef _H_CSV
#define _H_CSV

/*
 * csv readonly parse 
 * 1. 相邻字段必须被单个逗号分隔开, CRLF 换行
 * 2. 每个被嵌入的双引号字符必须被表示为两个双引号字符
 * 3. 字段都可以被双引号包裹, 有回车换行符双引号或者逗号, 那必须包裹
 */

#include <stdext.h>
#include <assext.h>

typedef struct {   // struct in heap
    int    rlen;   // 数据行数, 索引 [0, rlen)
    int    clen;   // 数据列数, 索引 [0, clen)
    char * data[]; // 保存数据, rlen * clen '二维数组'
} * csv_t;

//
// csv_get - 获取某个位置的对象内容
// csv     : csv_t 对象, create 返回的
// r       : 查找的行索引 [0, csv->rlen)
// c       : 查找的列索引 [0, csv->clen)
// return  : 返回这一项中内容, 后面可以用 atoi, atof, strdup ...
//
inline const char * csv_get(csv_t csv, int r, int c) {
    DCODE({
        if (!csv || r < 0 || r >= csv->rlen || c < 0 || c >= csv->clen) {
            RETNUL("params is error csv:%p, r:%d, c:%d.", csv, r, c);
        }
    });

    // 返回最终结果
    return csv->data[r * csv->clen + c];
}

//
// csv_delete - 释放由 csv_create 构建的对象
// csv     : csv_create 返回对象
// return  : void 
//
inline void csv_delete(csv_t csv) {
    free(csv);
}

//
// csv_create - 从文件中构建 csv 对象
// path    : csv文件内容
// return  : 返回构建好的 csv_t 对象
//
extern csv_t csv_create(const char * path);

#endif//_H_CSV
