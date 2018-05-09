#ifndef _H_PIPE
#define _H_PIPE

#include <socket.h>

#ifdef _MSC_VER

//
// pipe - 移植 linux 函数, 通过 WinSock 实现
// pipefd   : 索引 0 表示 recv fd, 1 是 send fd
// return   : 0 is success ,-1 is error returned
// 
extern int pipe(socket_t pipefd[2]);

//
// pipe_t - 通道类型
// pipe_close - 通道关闭操作
//
typedef HANDLE pipe_t;
#define pipe_close      CloseHandle

#else

typedef int pipe_t;
#define pipe_close      close

#endif

// chan_t - 管道类型
typedef struct { pipe_t recv, send; } chan_t[1];

//
// chan_open - 打开一个管道
// ch       : 待打开的管道
// return   : 0 is success ,-1 is error returned
//
extern int chan_open(chan_t ch);

// chan_close - 管道关闭
inline void chan_close(chan_t ch) {
    pipe_close(ch->recv);
    pipe_close(ch->send);
}

// chan_recv - 管道阻塞接收, 在 PIPE_BUF 4K 内原子交换
// chan_send - 管道阻塞发送, 
extern int chan_recv(chan_t ch, void * data, size_t len);
extern int chan_send(chan_t ch, const void * data, size_t len);

#endif//_H_PIPE
