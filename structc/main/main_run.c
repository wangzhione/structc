#include <sdos.h>
#include <chead.h>

//
// main_run - 业务运行的主体
// 通过 _DEBUG -> main_test
//
void main_run(int argc, char * argv[]) {
#ifdef _DEBUG
    TEST_FUNC(main_test);
#endif
    
    //
    // 开始你的表演 ... 
    //

}
