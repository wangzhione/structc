#ifndef _H_LOG
#define _H_LOG

#include <times.h>

//
// 有些 朴实 log, 也许很快很安全 ~
//
#define LOG_ERROR(fmt, ...) LOG_PRINTF("[ERROR]", fmt, ##__VA_ARGS__)
#define LOG_INFOS(fmt, ...) LOG_PRINTF("[INFOS]", fmt, ##__VA_ARGS__)
#if defined(_DEBUG)
#define LOG_DEBUG(fmt, ...) LOG_PRINTF("[DEBUG]", fmt, ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...) /*  (^_−)☆ */
#endif

//
// LOG_PRINTF - 拼接构建输出的格式串,最后输出数据
// pres     : 日志前缀串 必须 "" 包裹
// fmt      : 自己要打印的串,必须是双引号包裹. 
// return   : 返回待输出的串详细内容
//
#define LOG_PRINTF(pres, fmt, ...) \
log_printf(pres "[%s:%s:%d]" fmt "\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__)

//
// log_printf - 具体输出日志内容
// fmt      : 必须双引号包裹起来的串
// ...      : 对映fmt参数
// return   : void
//
void log_printf(const char * fmt, ...);

#endif//_H_LOG
