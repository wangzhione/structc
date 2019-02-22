#include "buf.h"

#define BUF_INT         (128)

//
// msg buffer manager
//
struct msg_buf {
    uint32_t sz;    // q.sz is msg header body size length
    char * data;    // 数据流
    int cap;        // 容量
    int len;        // 长度
};

// msg_buf_expand - 内存扩充 
static void msg_buf_expand(struct msg_buf * q, int sz) {
    // 确定内存是否足够
    int cap = q->cap, len = q->len;
    if (len + sz <= cap)
        return;

    // 开始构建所需内存
    do cap <<= 1; while (len + sz > cap);
    q->data = realloc(q->data, cap);
    assert(q->data != NULL);
    q->cap = cap;
}

//
// msg_buf_create - msg buffer 创建
// return   : msg buffer
//
inline msg_buf_t 
msg_buf_create(void) {
    struct msg_buf * q = malloc(sizeof(struct msg_buf));
    q->sz = 0;
    q->data = malloc(q->cap = BUF_INT);
    q->len = 0;
    return q;
}

//
// msg_buf_delete - msg buffer 删除
// q        : msg buffer
// return   : void
//
inline void 
msg_buf_delete(msg_buf_t q) {
    if (q) {
        free(q->data);
        free(q);
    }
}

//
// msg_buf_push - msg buffer push data
// q        : msg buffer
// data     : 内存数据
// sz       : 内存数据 size
//
inline static void msg_buf_push(msg_buf_t q, 
                                const void * data, int len) {
    msg_buf_expand(q, len);
    memcpy(q->data + q->len, data, len);
    q->len += len; 
}

// msg_buf_pop_data - q pop len data
inline void msg_buf_pop_data(msg_buf_t q, 
                             void * data, int len) {
    memcpy(data, q->data, len);
    q->len -= len;
    memmove(q->data, q->data + len, q->len);
}

// msg_buf_pop_sz - q pop sz
inline void msg_buf_pop_sz(msg_buf_t q) {
    msg_buf_pop_data(q, &q->sz, sizeof(uint32_t));
    q->sz = ntoh(q->sz);
}

//
// msg_buf_pop - msg buffer pop
// q        : msg buffer 
// p        : return msg
// return   : EParse 协议解析错误, ESmall 协议不完整
//
int msg_buf_pop(msg_buf_t q, msg_t * p) {
    // step 1 : 报文长度 buffer q->sz check
    if (q->sz <= 0 && q->len >= sizeof(uint32_t))
        msg_buf_pop_sz(q);
    // step 2 : check data parse is true
    int len = MSG_LEN(q->sz);
    if (len <= 0 && q->sz > 0) {
        *p = NULL;
        return EParse;
    }

    // step 3 : q->sz > 0 继续看是否有需要的报文内容
    if (len <= 0 || len > q->len) {
        *p = NULL;
        return ESmall;
    }

    // step 4: 索要的报文长度存在, 开始构建返回
    msg_t msg = malloc(sizeof(*msg) + len);
    msg->sz = q->sz;
    msg_buf_pop_data(q, msg->data, len);
    q->sz = 0;

    *p = msg;
    return SBase;
}

// msg_data_pop - data pop msg 
static msg_t msg_buf_data_pop(msg_buf_t q, 
                              const char * data, uint32_t n) {
    // step 1 : 报文长度 buffer q->sz init
    uint32_t sz;
    memcpy(&sz, data, sizeof sz);
    sz = ntoh(sz);

    // step 2 : check data len is true
    uint32_t len = MSG_LEN(q->sz);
    if (len <= 0 || len + sizeof(uint32_t) > n)
        return NULL;

    // step 3 : create msg
    msg_t msg = malloc(sizeof(*msg) + len);
    msg->sz = sz;
    memcpy(msg->data, data + sizeof(uint32_t), len);

    // step 4 : 数据存在, 填入剩余数据
    len += sizeof(uint32_t);
    if (len < n) {
        msg_buf_push(q, data + len, n - len);
    }

    return msg;
}

//
// buf_append - msg buffer 添加数据, 并尝试解析出结果
// q        : msg buffer
// data     : 内存数据
// sz       : 内存数据 size
// p        : return msg
// return   : EParse 协议解析错误, ESmall 协议不完整
//
int 
msg_buf_append(msg_buf_t q,
               const void * data, uint32_t sz,
               msg_t * p) {
    DCODE({
        if(!q || !data || !sz || !p) {
            EXIT(
                "error q = %p, data = %p, sz = %u, p = %p\n", 
                q, data, sz, p
            );
        }
    });

    // data, sz 刚好可以解析出 msg 情况处理
    if (q->sz <= 0 && sz > BUF_INT) {
        *p = msg_buf_data_pop(q, data, sz);
        if (*p)
            return SBase;
    }

    msg_buf_push(q, data, sz);
    return msg_buf_pop(q,p);
}
