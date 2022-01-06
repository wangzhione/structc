#include "base.h"

//
// main     - 随风奔跑, 追逐闪电 ~ 
// return   : 状态码 0 is success
//
int main(void) {
    // 初始化 ~ 虎
    // 
    EXTERN_RUN(init);

    // main test 单元测试消除 ~ 魅
    // Release build: make D=-DNDEBUG
    //
#if !defined(NDEBUG) || defined(_DEBUG)
    EXTERN_RUN(test);
#endif

    // ... 执行业务主 ~ 龍
    //
    EXTERN_RUN(exec);

    exit(EXIT_SUCCESS);
}
