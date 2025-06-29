#pragma once

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

//
// ~ 力求最小时间业务单元 ~ 
// 1s 秒 = 1000ms 毫秒 = 1000000us 微秒 = 1000000000ns 纳秒
//
// const (
//     Nanosecond  time_t   = 1
//     Microsecond          = 1000 * Nanosecond
//     Millisecond          = 1000 * Microsecond
//     Second               = 1000 * Millisecond
//     Minute               = 60 * Second
//     Hour                 = 60 * Minute
// )

#if defined(__linux__) && defined(__GNUC__)

#include <unistd.h>
#include <sys/time.h>

#endif

/* A year not divisible by 4 is not leap.
 * If div by 4 and not 100 is surely leap.
 * If div by 100 *and* not by 400 is not leap.
 * If div by 100 and 400 is leap. */
inline bool is_leap_year(time_t year) {
    return ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0));
}

/* We use a private localtime implementation which is fork-safe. The logging
 * function of Redis may be called from other threads. */
extern void localtime_get(struct tm * restrict p, time_t t);

// times_t - 时间串类型
// len = 33 + 1, buf = 2022-03-26 15:38:29.934089326 CST
// len = 53 + 1, buf = 2022年03月26日 15时38分29秒.934071664纳秒 CST
typedef char times_t[64];

//
// times_get - 解析时间串, 返回时间戳
// ns       : 时间串内容 
// out      : 返回得到的时间戳
// outm     : 返回得到的时间结构体
// return   : 返回 true 表示构造成功
//
extern bool times_get(times_t ns, time_t * out, struct tm * outm);

//
// time_get - 解析时间串, 返回时间戳
// ns       : 时间串内容  
// return   : < 0 is error
//
extern time_t time_get(times_t ns);

//
// time_day_equal - 判断时间戳是否是同一天
// n        : 第一个时间戳
// t        : 第二个时间戳
// return   : true 表示同一天
//
extern bool time_day_equal(time_t n, time_t t);

//
// time_week_equal - 判断时间戳是否是同一周
// n        : 第一个时间戳
// t        : 第二个时间戳
// return   : true 表示同一周
//
extern bool time_week_equal(time_t n, time_t t);

//
// times_day_equal - 判断时间串是否是同一天
// ns       : 第一个时间串
// ts       : 第二个时间串
// return   : true 表示同一天
//
extern bool times_day_equal(times_t ns, times_t ts);

//
// times_week_equal - 判断时间串是否是同一周
// ns       : 第一个时间串
// ts       : 第二个时间串
// return   : true 表示同一周
//
extern bool times_week_equal(times_t ns, times_t ts);

// TIMES_FMT_STR - "{年}.{月}.{日}.{时}.{分}.{秒}.{纳秒} {时区}"
#define TIMES_FMT_STR "%04d-%02d-%02d %02d:%02d:%02d.%09ld %s"

//
// times_fmt - 通过 fmt 格式最终拼接一个字符串
// fmt      : 推荐遵循 TIMES_FMT_STR 意图
// out      : 最终保存的内容
// sz       : buf 长度
// return   : 返回生成串长度
//
int times_fmt(const char * fmt, char out[], size_t sz);

//
// times_buf - 存储带毫秒时间串 "2016-07-10 22:38:34 500"
// ns       : 返回生成串
// return   : 返回生成串长度
//
inline int times_buf(times_t ns) {
    return times_fmt(TIMES_FMT_STR, ns, sizeof(times_t));
}

// times TLS time str 版本
extern const char * times(void);
