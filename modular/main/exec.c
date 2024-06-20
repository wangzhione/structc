#include "base.h"

#include "config/config.h"

//
// exec     - 执行业务 🐖🐍🐉
// return   : void
//
void exec(void) {
    //
    // .... .. . 开始你的表演
    //
    struct config * conf = config_instance();

    PERR("I'm here. Where are you!");

    puts(conf->description);
    puts(conf->image);
}
