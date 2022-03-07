﻿#include <buf.h>

void msg_test(void) {
    uint32_t x = 0x12345678;
    uint8_t * ptr = (uint8_t *)&x;
    printf("0x %x %x %x %x\n", ptr[0], ptr[1], ptr[2], ptr[3]);

    x = big(x);
    printf("0x %x %x %x %x\n", ptr[0], ptr[1], ptr[2], ptr[3]);

    x = big(x);
    printf("0x %x %x %x %x\n", ptr[0], ptr[1], ptr[2], ptr[3]);
}

// small - 转本地字节序(小端)
inline uint32_t small(uint32_t x) {
# ifdef ISBIG
    uint8_t t;
    uint8_t * p = (uint8_t *)&x;

    t = p[0]; p[0] = p[3]; p[3] = t;
    t = p[1]; p[1] = p[2]; p[2] = t;
# endif
    return x;
}

//
// buf_test msg buf test
//
void buf_test(void) {
    msg_buf_t q = msg_buf_create();

    // 构建数据填充进去
    char * str = "simplc";
    uint32_t len = (uint32_t)strlen(str) + 1;
    uint32_t x = MSG_SZ(1, len);
    x = small(x);
    printf("x = 0x%x, len = %u\n", x, MSG_LEN(x));

    // send阶段, 先发送字符长度
    msg_t msg;
    int ret = msg_buf_append(q, &x, sizeof(uint32_t), &msg);
    IF (ret == MSG_BUF_PARSE);
    if (ret == MSG_BUF_OK) {
        printf("ret = %d, type = %d, sz = %d, data = %s.\n", 
            ret, MSG_TYPE(msg->sz), MSG_LEN(msg->sz), msg->data);
    }
    printf("ret = %d, msg = %p.\n", ret, msg);

    // send 阶段, 后发送详细数据
    ret = msg_buf_append(q, str, len, &msg);
    IF (ret == MSG_BUF_PARSE);
    if (ret == MSG_BUF_OK) {
        printf("ret = %d, type = %d, sz = %d, data = %s.\n",
            ret, MSG_TYPE(msg->sz), MSG_LEN(msg->sz), msg->data);
        msg_delete(msg);
    }
    printf("ret = %d, msg = %p.\n", ret, msg);

    // 开始测试客户端给服务器发送消息
    msg_t m = msg_create(str, len);
    printf("m = %p, m->sz = %d, m->data = %s.\n", m, m->sz, m->data+4);
    ret = msg_buf_append(q, m->data, m->sz, &msg);
    msg_delete(m);
    IF (ret == MSG_BUF_PARSE);
    if (ret == MSG_BUF_OK) {
        printf("ret = %d, type = %d, sz = %d, data = %s.\n",
            ret, MSG_TYPE(msg->sz), MSG_LEN(msg->sz), msg->data);
        msg_delete(msg);
    }
    printf("ret = %d, msg = %p.\n", ret, msg);

    msg_buf_delete(q);
}
