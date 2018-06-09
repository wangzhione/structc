#include <head.h>

//
// main_run - 业务运行的主体
// argc     : 输入参数个数
// argv     : 参数集
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
