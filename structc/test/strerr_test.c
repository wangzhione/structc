#include <stdio.h>
#include <assext.h>
#include <strext.h>

//
// 根据 winerror.h 生成错误信息的模板
// 输入文件 : winerror.h.template
//

//
// 输出文件 : stderr.c.template
//

/*
#if defined(__GNUC__)

#include <string.h>

extern inline const char * strerr(int err) {
    return strerror(err);
}

#endif

#if defined(_MSC_VER)

#include <winerror.h>

#define DWORD int

extern const char * strerr(int err) {
    switch (err) {
    case ERROR_SUCCESS:                                                          : return "The operation completed successfully";
    >case >err<:                                                                 : return >str<;<
    }
    return "The aliens are coming. Go tell your favorite people"
}

#undef  DWORD int

#endif

 */

static inline FILE * _out_top(void) {
    const char * opath = "stderr.c.template";
    FILE * out = fopen(opath, "wb");
    if (NULL == out) {
        CERR_EXIT("foepn is err opath = %s", opath);
    }

    fputs(
"#if defined(__GNUC__)\n\
\n\
#include <string.h>\n\
\n\
extern inline const char * strerr(int err) {\n\
    return strerror(err);\n\
}\n\
\n\
#endif\n\
\n\
#if defined(_MSC_VER)\n\
\n\
#include <winerror.h>\n\
\n\
#define DWORD int\n\
\n\
extern const char * strerr(int err) {\n\
    switch (err) {\n"
        , out);

    return out;
}

static inline void _out_low(FILE * out) {
    fputs(
"    }\n\
\n\
    return \"The aliens are coming, Go tell your favorite people\";\n\
}\n\
\n\
#undef  DWORD\n\
\n\
#endif\n"
        , out);

    fclose(out);
}

static inline void _out_put(FILE * out, char buf[]) {
    fputs(buf, out);
}

//
// 假装在面向对象
//
#define out_top()       \
FILE * out = _out_top()
#define out_low()       \
_out_low(out)
#define out_put(buf)    \
_out_put(out, buf)

//
// 开始构造内容
//
void strerr_test(void) {
    const char * ipath = "winerror.h.template";
    // 打开文件处理
    char * txt = str_freads(ipath);
    if (NULL == txt) {
        RETNIL("str_freads ipath err = %s", ipath);
    }

    out_top();

    /*
     txt 中筛选出下面内容

     //
     // MessageId: ERROR_SUCCESS
     //
     // MessageText:
     //
     // The operation completed successfully.
     //
     #define ERROR_SUCCESS                    0L

     */

    // 开始匹配查找
    int i;
    char buf[1024] = "    case ", c;
    const char * tems = txt;
    const char * msgids = "// MessageId: ";
    size_t cnt = strlen(msgids);
    int pre = 82, idx = (int)strlen(buf) - 1;

    while ((tems = strstr(tems, msgids)) != NULL) {
        // 找到内容就开始清洗出 MessageId 和 MessageText
        const char * strs = tems + cnt;
        
        // 先获取 MessageId
        i = idx;
        while ((c = *strs++) != '\n') {
            if (c == ' ' || c == '(')
                break;
            buf[++i] = c;
        }
        // 清除多余字符
        if (c == ' ' || c == '(') {
            while (*strs++ != '\n')
                ;
        } else if (buf[i] != '\r') // 去除 \r
            i++;

        // 插入空格控制对齐
        while (i < pre)
            buf[i++] = ' ';
        buf[i++] = ':';
        buf[i++] = ' ';
        buf[i++] = 'r';
        buf[i++] = 'e';
        buf[i++] = 't';
        buf[i++] = 'u';
        buf[i++] = 'r';
        buf[i++] = 'n';
        buf[i++] = ' ';
        buf[i++] = '"';

        // 再获取 MessageText
        
        // step 1 到 :
        while (*strs++ != ':')
            ;
        // step 2 跳过 '/' '\n' '\r' ' ' 找到那个唯一的字符
        do {
            c = *strs++;
            if (c == '{') {
                /*
                    //
                    // MessageId: ERROR_FLOAT_MULTIPLE_TRAPS
                    //
                    // MessageText:
                    //
                    // {EXCEPTION}
                    // Multiple floating point traps.
                    //
                    #define ERROR_FLOAT_MULTIPLE_TRAPS       631L    // winnt
                 */
                while (*strs++ != '}')
                    ;
                c = *strs;
            }

        } while (c == '\r' || c == '\n' || c == ' ' || c == '/');

        // step 3 读取到 \n
        buf[i] = c;
        while ((c = *strs++) != '\n') {
            if (c == '"' || c == '\\')
                buf[++i] = '\\';
            else if (c == '%')
                buf[++i] = '%';
            buf[++i] = c;
        }
        // 去除 \r
        if (buf[i] == '\r')
            --i;

        if (buf[i] == ',') {
            // 还有一行继续读取, 清除掉部分内容
            do {
                c = *strs++;
            } while (c == '\r' || c == '\n' || c == ' ' || c == '/');

            // 继续读取内容
            while ((c = *strs++) != '\n') {
                if (c == '"' || c == '\\')
                    buf[++i] = '\\';
                else if (c == '%')
                    buf[++i] = '%';
                buf[++i] = c;
            }
            // 去除 \r
            if (buf[i] == '\r')
                --i;
        }

        // step 4 去除最后的 '.'
        if (buf[i] != '.')
            ++i;
        buf[i++] = '"';
        buf[i++] = ';';
        buf[i++] = '\n';
        buf[i] = '\0';

        out_put(buf);

        // 开始继续下一轮循环
        tems = strs;
    }

    out_low();

    free(txt);
}

/*

    最后删除下面内容:

    case E_NOTIMPL      : return "Not implemented";
    case E_OUTOFMEMORY  : return "Ran out of memory";
    case E_INVALIDARG   : return "One or more arguments are invalid";
    case E_NOINTERFACE  : return "No such interface supported";
    case E_POINTER      : return "Invalid pointer";
    case E_HANDLE       : return "Invalid handle";
    case E_ABORT        : return "Operation aborted";
    case E_FAIL         : return "Unspecified error";
    case E_ACCESSDENIED : return "General access denied error";

 */
