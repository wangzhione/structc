#define ALLOC_OFF
#include "alloc.h"

#define JEMALLOC_NO_DEMANGLE
#include <jemalloc/jemalloc.h>

// check 内存检测并处理
inline void * check(void * ptr, size_t size) {
    if (NULL == ptr) {
        fprintf(stderr, "check memory collapse %zu\n", size);
        fflush(stderr);
        abort();
    }
    return ptr;
}

//
// realloc_ - realoc 包装函数
// ptr      : 首地址, NULL 等同于 malloc
// size     : 重新分配的内存大小
// return   : 返回重新分配的新地址
//
inline void * realloc_(void * ptr, size_t size) {
    void * ntr = je_realloc(ptr, size);
    return check(ntr, size);
}

//
// calloc_  - calloc 包装函数
// num      : 数量
// size     : 大小
// return   : 返回可用内存地址, 并置 0
//
inline void * calloc_(size_t num, size_t size) {
    void * ptr = je_calloc(num, size);
    return check(ptr, size);
}

//
// malloc_  - malloc 包装函数
// size     : 分配的内存字节
// return   : 返回可使用的内存地址
//
inline void * malloc_(size_t size) {
    void * ptr = je_malloc(size);
    return check(ptr, size);
}

//
// free_    - free 包装函数
// ptr      : 内存首地址
// return   : void
//
inline void free_(void * ptr) {
    je_free(ptr);
}

//
// strdup_  - strdup 包装函数
// s        : '\0' 结尾 C 字符串
// return   : 拷贝后新的 C 字符串
//
inline char * strdup_(const char * s) {
    if (s) {
        size_t n = strlen(s) + 1;
        char * ptr = malloc_(n);
        return memcpy(ptr, s, n);
    }
    return NULL;
}
