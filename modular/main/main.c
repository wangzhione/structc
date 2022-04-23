#include "struct.h"

//
// main     - 随风奔跑, 追逐闪电 ~ 
// return   : 状态码 0 is success
//
int main(int argc, char * argv[]) {
    // 
    // 系统初始化 ~ 虎
    EXTERN_RUN(init, argc, argv);

    // 执行业务主 ~ 龍 ...
    //
    EXTERN_RUN(exec);

    exit(EXIT_SUCCESS);
}
