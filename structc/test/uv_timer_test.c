#include <uv.h>
#include <struct.h>

// 继承 uv_timer_t 结构
struct gravity {
    uv_timer_t tick;

    uv_tty_t tty;

    int width;
    int height;
    int pos;

    char * msg;
};

// update - 更新图片内容
void update(struct gravity * grav) {
    char data[BUFSIZ];
    uv_buf_t buf;
    buf.base = data;
    //
    // \033[2J      : 清屏
    // \033[H       : 光标移到(0, 0)
    // \033[%dB     : 光标下移 %d 行
    // \033[%dC     : 光标右移 %d 行
    // \033[42;37m  : 底色 41 绿底, 字色 37 白字
    //
    // \033[0m      : 关闭所有属性
    //
    buf.len = sprintf(data, "\033[2J\033[H\033[%dB\033[%dC\033[42;37m%s",
                            grav->pos,
                            (grav->width - (int)strlen(grav->msg)) / 2,
                            grav->msg);
    assert(buf.len < BUFSIZ);
    if (grav->pos == grav->height) {
        // 关闭屏幕额外属性
        const char * resets = "\033[0m";
        strcat(data, resets);
        buf.len += (int)strlen(resets);
    }

    // 写入消息
    uv_try_write((uv_stream_t *)&grav->tty, &buf, 1);

    // 当超过当前屏幕, 退出定时器
    if (++grav->pos > grav->height) {
        // 重置tty
        uv_tty_reset_mode();
        uv_timer_stop(&grav->tick);
    }
}

//
// uv_timer_test - 测试 timer 使用
//
void uv_timer_test(void) {
    uv_loop_t * loop = uv_default_loop();
    struct gravity grav = { { 0 } };

    uv_tty_init(loop, &grav.tty, 1, 0);
    uv_tty_set_mode(&grav.tty, UV_TTY_MODE_NORMAL);

    // 获取当前屏幕宽高信息
    if (uv_tty_get_winsize(&grav.tty, &grav.width, &grav.height)) {
        fprintf(stderr, "Could not get TTY information\n");
        uv_tty_reset_mode();
        return;
    }

    fprintf(stderr, "Width %d, height %d\n", grav.width, grav.height);
    
    // 启动 timer 刷新屏幕信息
    grav.msg = u8"我不甘心 ~";
    uv_timer_init(loop, &grav.tick);
    uv_timer_start(&grav.tick, (uv_timer_cb)update, 200, 200);
    
    uv_run(loop, UV_RUN_DEFAULT);
}
