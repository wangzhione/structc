#include "head.h"

//
// main_run - 运行的主函数
// argc     : 参数长度
// argv     : 参数数组
// return   : void
//
void main_run(int argc, char * argv[]) {
    //
    // .... ... .. . 开始你的表演
    //
    struct conf * conf = conf_instance();

    fprintf(stderr, "I am here\n");

    puts(conf->description);
    puts(conf->image);
}
