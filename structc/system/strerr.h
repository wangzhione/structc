#ifndef _STRERR_H
#define _STRERR_H

#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

//
// strerr - strerror 跨平台实现
// no      : linux errno, winds WSAGetLastError()
// return  : system os 拔下来的提示 message
//
extern const char * strerr(int no);

#ifdef _WIN32

#include <windows.h>

#undef  errno
#define errno                    (GetLastError())
#undef  strerror
#define strerror                 ((char * (*)(int))strerr)

#endif

#endif//_STRERR_H
