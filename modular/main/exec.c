#include "base.h"

//
// exec     - 执行业务 🐖🐍🐉
// return   : void
//
void exec(void) {
    //
    // .... .. . 开始你的表演
    //
    struct conf * conf = conf_instance();

    fprintf(stderr, "I am here\n");

    puts(conf->description);
    puts(conf->image);
}
