#ifndef _H_STRERR
#define _H_STRERR

#include "struct.h"

#ifdef _WIN32

#include <windows.h>

#undef  errno
#define errno                   (GetLastError())
#undef  strerror
#define strerror                ((char * (*)(int))strerr)

#endif

//
// strerr - strerror 跨平台实现版本
// err      : linux errno, winds 有 WSAGetLastError() ...
// return   : system os 拔下来的提示字符串
//
extern const char * strerr(int err);

#endif//_H_STRERR
