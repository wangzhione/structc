#include <head.h>

/*
 单元功能测试要求 : 
 
 编写规律 : 
 {接口}.h -> {接口}_test.c -> void {接口}_test(void) { ... }

 调用规律 :
 head.h :: TEST_FUNC({接口}_test) 放入 structc.c :: test_structc 

 */

//
// estring_test - 测试 estring_test.h 中接口设计
//
void strext_test(void) {
    unsigned hash = str_hash(STR(__func__)); 
    printf("%s -> %u\n", STR(__func__), hash);

	// 输出数据测试
	char * txt = str_freads("README.md");
    IF(txt == NULL);
    puts(txt);
    u82g(txt);
    // UTF-8 FILE with BOM
    puts(txt + 2);
    free(txt);

    // 开始字符串构建
    char * str = str_printf("你好吗 ? double = %0.4f\n", 1.2340);
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
}
