#include "head.h"

//
// main - 奔跑的起点
// argc     : 参数个数
// argv     : 参数数组
// return   : 状态码
//
int main(int argc, char * argv[]) {
    // 初始化 ... .
    // 
    EXTERN_RUN(main_init);

    // make D=-D_DEBUG
    // main_test 单元测试才会启动
    //
#ifdef _DEBUG
    EXTERN_RUN(main_test);
#endif

    // ... 
    // ... 项目运行的主函数 ~ 龍
    //
    EXTERN_RUN(main_run, argc, argv);

    return EXIT_SUCCESS;
}
