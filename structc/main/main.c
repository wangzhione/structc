#include "base.h"

//
// main     - 随风奔跑, 追逐闪电 ~ 
// argc     : 运行参数个数
// argv     : 运行参数数组
// return   : 状态码 0 is success
//
int main(int argc, char * argv[]) {
    // 初始化 ~ 虎
    // 
    EXTERN_RUN(init);

    // main test 单元测试消除 ~ 魅
    // make D=-DNDEBUG Release build
    //
#ifndef NDEBUG
    EXTERN_RUN(test);
#endif

    // ... 执行业务主 ~ 龍
    //
    EXTERN_RUN(exec, argc, argv);

    return EXIT_SUCCESS;
}
