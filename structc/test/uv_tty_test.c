#include <uv.h>
#include <assext.h>

// 简单的输出信息
static void _sendtty(uv_tty_t * tty, const char * ims) {
    uv_buf_t buf;
    uv_write_t req;

    assert(tty && ims);
    buf.base = (char *)ims;
    buf.len = (int)strlen(buf.base);

    // 具体发送消息
    uv_write(&req, (uv_stream_t *)tty, &buf, 1, NULL);
}

//
// 测试 libuv tty 操作控制台
//
void uv_tty_test(void) {
    uv_tty_t tty;
    uv_loop_t * loop = uv_default_loop();

    uv_tty_init(loop, &tty, 1, 0);
    uv_tty_set_mode(&tty, UV_TTY_MODE_NORMAL);

    if (uv_guess_handle(1) == UV_TTY) {
        _sendtty(&tty, "\033[41;37m");
    }

    _sendtty(&tty, "我的 喵酱 ((●'-'●)) 在哪呢~");

    // 重置控制台显示颜色
    uv_buf_t buf;
    uv_write_t req;
    buf.base = "\033[40;37m";
    buf.len = (int)strlen(buf.base);
    uv_write(&req, (uv_stream_t *)&tty, &buf, 1, NULL);

    // 重置终端行为
    uv_tty_reset_mode();
    
    uv_run(loop, UV_RUN_DEFAULT);
}
