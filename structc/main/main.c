#include "head.h"

//
// main - 奔跑一切的起点
// argc     : 参数长度
// argv     : 参数数组
// return   : 退出状态码
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
    // ... 启动项目运行的主函数
    //
    EXTERN_RUN(main_run, argc, argv);

    return EXIT_SUCCESS;
}
