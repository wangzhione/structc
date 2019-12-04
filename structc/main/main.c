#include "base.h"

//
// main     - 奔跑的起点
// argc     : 参数个数
// argv     : 参数数组
// return   : 状态码
//
int main(int argc, char * argv[]) {
    // 初始化 ... .
    // 
    EXTERN_RUN(init);

    // make D=-DNDEBUG
    // main test 单元测试才会启动
    //
#ifndef NDEBUG
    EXTERN_RUN(test);
#endif

    // ... 
    // ... 执行业务主 ~ 龍
    //
    EXTERN_RUN(exec, argc, argv);

    return EXIT_SUCCESS;
}
