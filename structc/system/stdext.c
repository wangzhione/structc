#include <jemalloc/jemalloc.h>

//
// 编译导入
// gcc -I./structc/system 编译链接不了 -ljemalloc
//
#define _NO_STDEXT_
#include "stdext.h"

//
// free_ - free 包装函数
// ptr      : 通过下面函数分配的内存, 必须通过这个函数销毁
// return   : void
//
inline void 
free_(void * ptr) {
    je_free(ptr);
}

// 简单错误信息打印
static inline void _ext(void * ptr, size_t size) {
    fprintf(stderr, "alloc error ptr = %p, size = %zu!\n", ptr, size);
    fflush(stderr);
    abort();
}

//
// malloc_ - malloc 包装, 封装一些业务特性代码
// size     : 分配的内存字节
// return   : 返回可使用的内存地址.
//
inline void * 
malloc_(size_t size) {
    void * ptr = je_malloc(size);
    if (NULL == ptr)
        _ext(ptr, size);
    return ptr;
}

//
// calloc_ - calloc 包装, 封装一些特殊业务
// num      : 数量
// size     : 大小
// return   : 返回可用内存地址, 并且置0
//
inline void * 
calloc_(size_t num, size_t size) {
    void * ptr = je_calloc(num, size);
    if (NULL == ptr)
        _ext(ptr, num * size);
    return ptr;
}

//
// realloc_ - realoc 包装函数, 封装一些特殊业务
// ptr      : 重新分配的内存首地址, NULL 等同于 malloc
// size     : 重新分配的内存大小
// return   : 返回重新分配好的新地址内容
//
inline void * 
realloc_(void * ptr, size_t size) {
    void * nptr = je_realloc(ptr, size);
    if (NULL == nptr) {
        je_free(ptr);   // 可有可无, 内存模块已经是未定义行为的边缘了
        _ext(ptr, size);   
    }
    return ptr;
}
