#include "struct/strext.h"
#include "chead.h"

// main_structc - 业务跑起来的实际主函数
void main_structc(void) {
    EXTERN_RUN(test_structc);
}

// test_structc - 整个测试业务主函数
void test_structc(void) {
    TEST_FUNC(test_strext);
}