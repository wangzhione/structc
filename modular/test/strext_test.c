﻿#include <base.h>
#include <utf8.h>

/*
 单元功能测试要求 : 
 
 编写规律 : 
 {接口}.h -> {接口}_test.c -> void {接口}_test(void) { ... }

 调用规律 :
 {接口}_test.c :: {接口}_test 放入 main::test.c :: test 

 */

//
// str_cmpi - 字符串不区分大小写比较函数
// ls       : 左串
// rs       : 右串
// return   : ls > rs 返回 > 0; ... < 0; ... =0
//
int str_cmpi(const char * ls, const char * rs) {
    int l, r;
    if (!ls || !rs) return (int)(ls - rs);
    do {
        if ((l = *ls++) >= 'A' && l <= 'Z')
            l += 'a' - 'A';
        if ((r = *rs++) >= 'A' && r <= 'Z')
            r += 'a' - 'A';
    } while (l == r && l);
    return l - r;
}

//
// str_cmpin - 字符串不区分小写的限定字符比较函数
// ls       : 左串
// rs       : 右串
// n        : 长度
// return   : ls > rs 返回 > 0; ... < 0; ... =0
//
int str_cmpin(const char * ls, const char * rs, size_t n) {
    int l, r;
    if (!ls || !rs || !n) return (int)(ls - rs);
    do {
        if ((l = *ls++) >= 'A' && l <= 'Z')
            l += 'a' - 'A';
        if ((r = *rs++) >= 'A' && r <= 'Z')
            r += 'a' - 'A';
    } while (--n > 0 && l == r && l);
    return l - r;
}

//
// strext_test - 测试 estring_test.h 中接口设计
//
void strext_test(void) {
    unsigned hash = BKDHash(STR(__func__)); 
    printf("%s -> %u\n", STR(__func__), hash);

	// 输出数据测试
	char * txt = str_freads("README.md");
    IF(txt == NULL);
    puts(txt);
    utf82gbk(txt);
    // UTF-8 FILE with BOM
    puts(txt + 2);
    free(txt);

    // 开始字符串构建
    int age = -2;
    char * str = str_sprintf("你好吗 ? double = %0.4f, d=%d\n", 1.2340, age);
    puts(str);
    free(str);

    // 开始判断空格
    char nsr[] = " 123   ";
    puts(nsr);

    // printf 从左向右执行
    char * nstr = str_trim(nsr);
    printf("nsr = %s, len = %zu\n", nstr, strlen(nstr));

    // 测试字符串匹配
    printf("str_cmpin = %d\n", str_cmpin("nUlL", "null;xxx", 4));

    // 测试数据复制
    char src[10] = "123456781";
    char tar[11] = "1234567891";

    int len = str_cpyn(src, tar, sizeof src);
    printf("len = %d, src = %s\n", len, src);

    // warning: ‘__builtin_strncpy’ output may be truncated copying 10 bytes from a string of length 10 [-Wstringop-truncation]
    // error 发生意外. src[sizeof src] '\0' 会被吃掉
    // 高性能领域推荐 memcpy
    // warning: ‘__builtin_strncpy’ output may be truncated copying 10 bytes from a string of length 10 [-Wstringop-truncation]
    // len = strlen(strncpy(src, tar, sizeof src));
    printf("len = %d, src = %s\n", len, src);
}
