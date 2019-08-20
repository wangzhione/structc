#ifndef _PIPE_H
#define _PIPE_H

#include "socket.h"

#ifdef _MSC_VER

//
// pipe - 移植 linux 函数, 通过 WinSock 实现
// pipefd   : 索引 0 表示 recv fd, 1 是 send fd
// return   : 0 is success ,-1 is error returned
//
extern int pipe(socket_t pipefd[2]);

// pass_t - 单通道类型
// pass_close - 单通道类型
typedef HANDLE pass_t;
#define pass_close      CloseHandle

#else

//
// 库设计的核心主要是包装 pipe
//
typedef int pass_t;
#define pass_close      close

#endif

// chan_t - 管道类型
typedef struct { pass_t recv, send; } pipe_t[1];

// pipe_close - 管道关闭
inline void pipe_close(pipe_t ch) {
    pass_close(ch->recv);
    pass_close(ch->send);
}

//
// pipe_open - pipe open
// ch       : 管道类型
// return   : 0 is success ,-1 is error returned
//
extern int pipe_open(pipe_t ch);

// pipe_recv - 管道阻塞接收, PIPE_BUF 4K 内原子交换
// pipe_send - 管道阻塞发送
extern int pipe_recv(pipe_t ch, void * buf, int sz);
extern int pipe_send(pipe_t ch, const void * buf, int sz);

#endif//_PIPE_H
