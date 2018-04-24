#include <stdio.h>
#include <assext.h>
#include <strext.h>

//
// 分析 winerror.h
// 输入 : winerror.h.template
//

//
// 输出 : stderr.c.template
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

inline FILE * out_top(void) {
    const char * opath = "stderr.c.template";
    FILE * out = fopen(opath, "wb");
    if (NULL == out) {
        EXIT("foepn is err opath = %s", opath);
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

inline void out_low(FILE * out) {
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

inline void out_put(FILE * out, char buf[]) {
    fputs(buf, out);
}

//
// 详细 strerr 测试内容
//
void strerr_test(void) {
    const char * ipath = "winerror.h.template";
    // 读取文件内容
    char * txt = str_freads(ipath);
    if (NULL == txt) {
        RETNIL("str_freads ipath err = %s", ipath);
    }

    FILE * out = out_top();

    /*
     txt 分析模板

     //
     // MessageId: ERROR_SUCCESS
     //
     // MessageText:
     //
     // The operation completed successfully.
     //
     #define ERROR_SUCCESS                    0L

     */

    int i;
    char buf[1024] = "    case ", c;
    const char * tems = txt;
    const char * msgids = "// MessageId: ";
    size_t cnt = strlen(msgids);
    int pre = 82, idx = (int)strlen(buf) - 1;

    while ((tems = strstr(tems, msgids)) != NULL) {
        // 分析 MessageId 和 MessageText
        const char * strs = tems + cnt;
        
        // 分析 MessageId
        i = idx;
        while ((c = *strs++) != '\n') {
            if (c == ' ' || c == '(')
                break;
            buf[++i] = c;
        }

        if (c == ' ' || c == '(') {
            while (*strs++ != '\n')
                ;
        } else if (buf[i] != '\r') // 处理 \r
            i++;

        // 插入部分 code
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

        // 分析 MessageText
        
        // step 1 找到 :
        while (*strs++ != ':')
            ;
        // step 2 找到 '/' '\n' '\r' ' ' 结尾部分
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

        // step 3 找到 \n
        buf[i] = c;
        while ((c = *strs++) != '\n') {
            if (c == '"' || c == '\\')
                buf[++i] = '\\';
            else if (c == '%')
                buf[++i] = '%';
            buf[++i] = c;
        }
        // 过滤 \r
        if (buf[i] == '\r')
            --i;

        if (buf[i] == ',') {
            do {
                c = *strs++;
            } while (c == '\r' || c == '\n' || c == ' ' || c == '/');

            while ((c = *strs++) != '\n') {
                if (c == '"' || c == '\\')
                    buf[++i] = '\\';
                else if (c == '%')
                    buf[++i] = '%';
                buf[++i] = c;
            }
            // 过滤 \r
            if (buf[i] == '\r')
                --i;
        }

        // step 4 过滤 '.'
        if (buf[i] != '.')
            ++i;
        buf[i++] = '"';
        buf[i++] = ';';
        buf[i++] = '\n';
        buf[i] = '\0';

        out_put(out, buf);

        // 继续处理
        tems = strs;
    }

    out_low(out);

    free(txt);
}

/*

    特殊情况处理

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
