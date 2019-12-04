#include "base.h"

//
// exec     - 执行业务 🐖🐍🐉
// argc     : 参数长度
// argv     : 参数数组
// return   : void
//
void exec(int argc, char * argv[]) {
    //
    // .... .. . 开始你的表演
    //
    struct conf * conf = conf_instance();

    fprintf(stderr, "I am here\n");

    puts(conf->description);
    puts(conf->image);
}
