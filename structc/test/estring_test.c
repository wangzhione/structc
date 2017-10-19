#include <chead.h>
#include <estring.h>

/*
 单元功能测试要求 : 
 
 编写规律 : 
 {接口}.h -> {接口}_test.c -> void {接口}_test(void) { ... }

 调用规律 :
 chead.h :: TEST_FUNC({接口}_test) 放入 structc.c :: test_structc 

 */

//
// estring_test - 测试 estring_test.h 中接口设计
//
void estring_test(void) {
    unsigned hash = str_hash(STR(__func__)); 
    printf("%s -> %u\n", STR(__func__), hash);
}
