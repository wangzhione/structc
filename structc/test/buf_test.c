#include <buf.h>

//
// buf_test msg buf test
//
void buf_test(void) {
    msg_buf_t q = msg_buf_create();

    // 构建数据填充进去
    char * str = "simplc";
    uint32_t len = (uint32_t)strlen(str) + 1;
    uint32_t x = MSG_SZ(1, len);
    x = hton(x);

    // send阶段, 先发送字符长度
    msg_t msg;
    int ret = msg_buf_append(q, &x, sizeof(uint32_t), &msg);
    IF (ret == EParse);
    if (ret == SBase) {
        printf("ret = %d, type = %d, sz = %d, data = %s.\n", 
            ret, MSG_TYPE(msg->sz), MSG_LEN(msg->sz), msg->data);
    }
    printf("ret = %d, msg = %p.\n", ret, msg);

    // send 阶段, 后发送详细数据
    ret = msg_buf_append(q, str, len, &msg);
    IF (ret == EParse);
    if (ret == SBase) {
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
    IF (ret == EParse);
    if (ret == SBase) {
        printf("ret = %d, type = %d, sz = %d, data = %s.\n",
            ret, MSG_TYPE(msg->sz), MSG_LEN(msg->sz), msg->data);
        msg_delete(msg);
    }
    printf("ret = %d, msg = %p.\n", ret, msg);

    msg_buf_delete(q);
}
