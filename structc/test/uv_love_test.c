#include <uv.h>
#include <chead.h>
#include <assext.h>

struct loves {
    uv_timer_t tick;

    uv_tty_t tty;

    int width;
    int height;
    int pos;

    char ** msgs;
    int len;
};

static char * _figure[] = {
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

// _love_stty : 内部发送消息
static inline _love_stty(struct loves * love, const char * msg) {
    uv_buf_t buf;
    buf.base = (char *)msg;
    buf.len = (int)strlen(buf.base);
    uv_try_write((uv_stream_t *)&love->tty, &buf, 1);
}

// _loves_init : 初始化当前 tty 结构
static void _loves_init(struct loves * love) {
    uv_loop_t * loop = uv_default_loop();
    memset(love, 0, sizeof *love);

    // 初始化 tty 环境
    uv_tty_init(loop, &love->tty, 1, 0);
    uv_tty_set_mode(&love->tty, UV_TTY_MODE_NORMAL);

    // 只对 tty 输出处理
    if (uv_guess_handle(1) != UV_TTY)
        CERR_EXIT("uv_guess_handle(1) != UV_TTY!");

    // 获取当前屏幕宽高信息
    if (uv_tty_get_winsize(&love->tty, &love->width, &love->height)) {
        uv_tty_reset_mode();
        CERR_EXIT("Could not get TTY information");
    }

    // 设置具体内容
    love->msgs = _figure;
    love->len = LEN(_figure);

    // 初始化定时器
    uv_timer_init(loop, &love->tick);
}

// _love_stty_screem : 屏幕输出内容
static void _love_stty_screem(struct loves * love) {
    char buf[BUFSIZ];
    int cnt = love->pos < love->len ? love->pos : love->len;

    // 重置索引位置
    int idx = love->height - love->pos;
    snprintf(buf, LEN(buf), "\033[2J\033[%dB", idx);
    _love_stty(love, buf);

    // 全部显示
    for (idx = 0; idx < cnt; idx++)
        _love_stty(love, love->msgs[idx]);
}

// _update - 更新刷新事件
static void _update(struct loves * love) {
    ++love->pos;

    // 开始绘制内容
    _love_stty_screem(love);

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
    struct loves love;
    _loves_init(&love);

    // 开始初始化, 定时器刷新事件
    uv_timer_start(&love.tick, (uv_timer_cb)_update, 200, 200);

    // 事件启动起来
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
