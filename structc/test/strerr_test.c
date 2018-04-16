#include <stdio.h>
#include <assext.h>
#include <strext.h>

//
// ���� winerror.h ���ɴ�����Ϣ��ģ��
// �����ļ� : winerror.h.template
//

//
// ����ļ� : stderr.c.template
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
// ��װ���������
//
#define out_top()       \
FILE * out = _out_top()
#define out_low()       \
_out_low(out)
#define out_put(buf)    \
_out_put(out, buf)

//
// ��ʼ��������
//
void strerr_test(void) {
    const char * ipath = "winerror.h.template";
    // ���ļ�����
    char * txt = str_freads(ipath);
    if (NULL == txt) {
        RETNIL("str_freads ipath err = %s", ipath);
    }

    out_top();

    /*
     txt ��ɸѡ����������

     //
     // MessageId: ERROR_SUCCESS
     //
     // MessageText:
     //
     // The operation completed successfully.
     //
     #define ERROR_SUCCESS                    0L

     */

    // ��ʼƥ�����
    int i;
    char buf[1024] = "    case ", c;
    const char * tems = txt;
    const char * msgids = "// MessageId: ";
    size_t cnt = strlen(msgids);
    int pre = 82, idx = (int)strlen(buf) - 1;

    while ((tems = strstr(tems, msgids)) != NULL) {
        // �ҵ����ݾͿ�ʼ��ϴ�� MessageId �� MessageText
        const char * strs = tems + cnt;
        
        // �Ȼ�ȡ MessageId
        i = idx;
        while ((c = *strs++) != '\n') {
            if (c == ' ' || c == '(')
                break;
            buf[++i] = c;
        }
        // ��������ַ�
        if (c == ' ' || c == '(') {
            while (*strs++ != '\n')
                ;
        } else if (buf[i] != '\r') // ȥ�� \r
            i++;

        // ����ո���ƶ���
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

        // �ٻ�ȡ MessageText
        
        // step 1 �� :
        while (*strs++ != ':')
            ;
        // step 2 ���� '/' '\n' '\r' ' ' �ҵ��Ǹ�Ψһ���ַ�
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

        // step 3 ��ȡ�� \n
        buf[i] = c;
        while ((c = *strs++) != '\n') {
            if (c == '"' || c == '\\')
                buf[++i] = '\\';
            else if (c == '%')
                buf[++i] = '%';
            buf[++i] = c;
        }
        // ȥ�� \r
        if (buf[i] == '\r')
            --i;

        if (buf[i] == ',') {
            // ����һ�м�����ȡ, ����������
            do {
                c = *strs++;
            } while (c == '\r' || c == '\n' || c == ' ' || c == '/');

            // ������ȡ����
            while ((c = *strs++) != '\n') {
                if (c == '"' || c == '\\')
                    buf[++i] = '\\';
                else if (c == '%')
                    buf[++i] = '%';
                buf[++i] = c;
            }
            // ȥ�� \r
            if (buf[i] == '\r')
                --i;
        }

        // step 4 ȥ������ '.'
        if (buf[i] != '.')
            ++i;
        buf[i++] = '"';
        buf[i++] = ';';
        buf[i++] = '\n';
        buf[i] = '\0';

        out_put(buf);

        // ��ʼ������һ��ѭ��
        tems = strs;
    }

    out_low();

    free(txt);
}

/*

    ���ɾ����������:

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
