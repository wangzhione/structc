#include <log.h>

void log_test(void) {
    LOG_ERROR("你好, a = %d, str = %s", 1, "asdf");
    LOG_INFOS("你好, a = %d, str = %s", 2, "asdf");
    LOG_DEBUG("你好, a = %d, str = %s", 3, "asdf");
}