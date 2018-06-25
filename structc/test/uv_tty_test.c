#include <uv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// 测试 libuv tty 操作控制台
// 输出一段有颜色的文字
//
void uv_tty_test(void) {
    uv_tty_t tty;
    uv_buf_t buf[3];
    unsigned i, len = sizeof buf / sizeof *buf;
    uv_loop_t * loop = uv_default_loop();

    // 目前只对 tty 控制台处理
    if (uv_guess_handle(1) != UV_TTY) {
        fprintf(stderr, "uv_guess_handle(1) != UV_TTY!\n");
        exit(EXIT_FAILURE);
    }

    uv_tty_init(loop, &tty, 1, 0);
    uv_tty_set_mode(&tty, UV_TTY_MODE_NORMAL);

    // 开始发送消息
    buf[0].base = "\033[46;37m";
    buf[1].base = u8"(✿◡‿◡) 喵酱 ((●'-'●)) 比 ♥ 里~ \n";
    buf[2].base = "\033[0m";
    for (i = 0; i < len; ++i)
        buf[i].len = (int)strlen(buf[i].base);
    uv_try_write((uv_stream_t *)&tty, buf, len);

    // 重置终端行为
    uv_tty_reset_mode();
    uv_run(loop, UV_RUN_DEFAULT);
}
