#include "log.h"

FILE * log_instance;

// log_init 日志库初始化
void log_init(const char * path) {
    if ((log_instance = fopen(path, "ab")) == nullptr) {
        // log 初始化失败, 程序默认启动失败.
        fprintf(stderr, "fopen ab error %"PRId64", %s\n", time(nullptr), path);
        exit(EXIT_FAILURE);
    }
}
