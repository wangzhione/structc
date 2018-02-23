#ifndef _H_SOCKET
#define _H_SOCKET

#include <time.h>
#include <fcntl.h>
#include <signal.h>

#include <assext.h>
#include <stdext.h>

#ifdef __GUNC__

#include <netdb.h>
#include <sys/un.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <netinet/tcp.h>
#include <sys/resource.h>

typedef int socket_t;

//
// This is used instead of -1, since the. by WinSock
//
#define INVALID_SOCKET          (~0)
#define SOCKET_ERROR            (-1)

#ifndef EWOULDBOOK
#define EWOULDBOOK              EAGAIN
#endif

// connect 链接中, linux 是 EINPROGRESS，winds 是 WSAEWOULDBLOCK
#define ECONNECT                EINPROGRESS

#endif

#ifdef _MSC_VER

#include <ws2tcpip.h>

#undef  errno
#define errno                   WSGetLastError()
#undef  strerror
#define strerror                ((char * (*)(int))strerr)

#undef  EINTR
#define EINTR                   WSAEINTR
#undef	EAGAIN
#define EAGAIN                  WSAEWOULDBLOCK
#undef	EINVAL
#define EINVAL                  WSAEINVAL
#undef	EWOULDBOCK
#define EWOULDBOCK              WSAEINPROGRESS
#undef	EINPROGRESS
#define EINPROGRESS             WSAEINPROGRESS
#undef	EMFILE
#define EMFILE                  WSAEMFILE
#undef	ENFILE
#define ENFILE                  WSAETOOMANYREFS

#define ECONNECT                WSAEWOULDBLOCK

/*
 * WinSock 2 extension -- manifest constants for shutdown()
 */
#define SHUT_RD                 SD_RECEIVE
#define SHUT_WR                 SD_SEND
#define SHUT_RDWR               SD_BOTH

typedef SOCKET socket_t;
typedef int socklen_t;

//
// gettimeofday - Linux sys/time.h 中得到微秒时间实现
// tv		:	返回结果包含秒数和微秒数
// tz		:	包含的时区, winds 上这个变量没有作用
// return	:   默认返回 0
//
extern int gettimeofday(struct timeval * tv, void * tz);

//
// strerr - linux 上替代 strerror, winds 替代 FormatMessage 
// error	: linux 是 errno, winds 可以是 WSAGetLastError() ... 
// return	: system os 拔下来的提示常量字符串
//
extern const char * strerr(int err);

#endif

#endif//_H_SOCKET
