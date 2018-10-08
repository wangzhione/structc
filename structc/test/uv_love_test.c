#include <uv.h>
#include <head.h>

struct love {
    uv_timer_t tick;

    uv_tty_t tty;

    int width;
    int height;
    int pos;

    char ** msgs;
    int len;
};

static char * figure[] = {
    u8"  背影 :- 汪国真\n",
    u8"  \n",
    u8"  背影\n",
    u8"  总是很简单\n",
    u8"  简单\n",
    u8"  是一种风景\n",
    u8"  \n",
    u8"  背影\n",
    u8"  总是很年轻\n",
    u8"  年轻\n",
    u8"  是一种清明\n",
    u8"  \n",
    u8"  背影\n",
    u8"  总是很含蓄\n",
    u8"  含蓄\n",
    u8"  是一种魅力\n",
    u8"  \n",
    u8"  背影\n",
    u8"  总是很孤零\n",
    u8"  孤零\n",
    u8"  更让人记得清\n"
};

// love_stty : 内部发送消息
static inline void love_stty(struct love * love, const char * msg) {
    uv_buf_t buf;
    buf.base = (char *)msg;
    buf.len = (int)strlen(buf.base);
    uv_try_write((uv_stream_t *)&love->tty, &buf, 1);
}

// love_init : 初始化当前 tty 结构
static void love_init(struct love * love) {
    uv_loop_t * loop = uv_default_loop();
    memset(love, 0, sizeof *love);

    // 初始化 tty 环境
    uv_tty_init(loop, &love->tty, 1, 0);
    uv_tty_set_mode(&love->tty, UV_TTY_MODE_NORMAL);

    // 只对 tty 输出处理
    if (uv_guess_handle(1) != UV_TTY)
        EXIT("uv_guess_handle(1) != UV_TTY!");

    // 获取当前屏幕宽高信息
    if (uv_tty_get_winsize(&love->tty, &love->width, &love->height)) {
        uv_tty_reset_mode();
        EXIT("Could not get TTY information");
    }

    // 设置具体内容
    love->msgs = figure;
    love->len = LEN(figure);

    // 初始化定时器
    uv_timer_init(loop, &love->tick);
}

// love_screem : 屏幕绘制内容
static void love_screem(struct love * love) {
    char buf[BUFSIZ];
    int cnt = love->pos < love->len ? love->pos : love->len;

    // 重置索引位置
    int idx = love->height - love->pos;
    snprintf(buf, LEN(buf), "\033[2J\033[H\033[%dB", idx);
    love_stty(love, buf);

    // 全部显示
    for (idx = 0; idx < cnt; idx++)
        love_stty(love, love->msgs[idx]);
}

// _update - 更新刷新事件
static void love_update(struct love * love) {
    ++love->pos;

    // 开始绘制内容
    love_screem(love);

    // 运行结束直接返回
    if (love->pos >= love->height) {
        // 重置tty
        uv_tty_reset_mode();
        uv_timer_stop(&love->tick);
    }
}

//
// uv_love_test - 情怀 ~
//
void uv_love_test(void) {
    struct love love;
    love_init(&love);

    // 开始初始化, 定时器刷新事件
    uv_timer_start(&love.tick, (uv_timer_cb)love_update, 200, 200);

    // 事件启动起来
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
