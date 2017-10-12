#include "../struct/strext.h"
#include "../chead.h"

/*
 单元功能测试要求 : 
 
 编写规律 : 
 {接口}.h -> test_{接口}.c -> void test_{接口}(void) { ... }

 调用规律 :
 chead.h :: TEST_FUNC(test_{接口}) 放入 structc.c :: test_structc 

 */

// test_strext - 测试 strext.h 中接口设计
void test_strext(void) {
    unsigned hash = str_hash(CSTR(__func__)); 
    printf("%s -> %u\n", CSTR(__func__), hash);
}