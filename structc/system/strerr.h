#pragma once

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#ifdef _WIN32

#include <windows.h>

#undef  errno
#define errno                    (GetLastError())

#endif

//
// strerr - strerror 线程安全跨平台实现
// no      : linux errno, winds WSAGetLastError()
// return  : system os 拔下来的提示 message
//
extern const char * strerr(int no);
