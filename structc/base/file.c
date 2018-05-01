#include <file.h>
#include <atom.h>
#include <assext.h>

//
// removes - 删除非空目录 or 文件
// path     : 文件全路径
// return   : < 0 is error, >=0 is success
//
inline int removes(const char * path) {
    char s[BUFSIZ];

#ifndef STR_RMRF
#   ifdef _MSC_VER
#       define STR_RMRF    "rmdir /s /q \"%s\""
#   else
#       define STR_RMRF    "rm -rf '%s'"
#   endif
#endif

    // path 超过缓冲区长度, 返回异常
    if (snprintf(s, sizeof s, STR_RMRF, path) == sizeof s) 
        return -1;
    return access(path, F_OK) ? 0 : -system(s);
}

//
// mkdirs - 创建多级目录
// path     : 目录路径
// return   : < 0 is error, 0 is success
//
int 
mkdirs(const char * path) {
    char c, * p, * s;

    // 参数错误直接返回
    if (!path || !*path) return -2;
    // 文件存在 or 文件一次创建成功 直接返回
    if (!access(path, F_OK) || !mkdir(path))
        return 0;

    // 跳过第一个 ['/'|'\\'] 检查是否是多级目录
    p = (char *)path;
    while ((c = *++p) != '\0')
        if (c == '/' || c == '\\')
            break;
    if (c == '\0') return -1;

    // 开始循环构建多级目录
    s = p = strdup(path);
    while ((c = *++p) != '\0') {
        if (c == '/' || c == '\\') {
            *p = '\0';

            if (access(s, F_OK)) {
                // 文件不存在, 开始创建, 创建失败直接返回错误
                if (mkdir(s)) {
                    free(s);
                    return -1;
                }
            }

            *p = c;
        }
    }

    // 最后善尾
    c = p[-1]; free(s);
    if (c == '/' || c == '\\')
        return 0;

    // 剩下最后文件路径, 开始构建
    return mkdir(path) ? -1 : 0;
}

struct file {
    time_t last;            // 文件最后修改时间点
    char * path;            // 文件全路径
    unsigned hash;          // 文件路径 hash 值

    file_f func;            // 执行行为
    void * arg;             // 行为参数

    struct file * next;     // 文件下一个结点
};

static struct files {
    atom_t lock;            // 当前对象原子锁
    struct file * head;     // 当前文件对象集
} _s;

// files add 
static void _add(const char * p, unsigned h, file_f func, void * arg) {
    struct file * fu;
    if (mtime(p) == -1) {
        RETNIL("mtime error p = %s", p);
    }

    fu = malloc(sizeof(struct file));
    fu->last = -1;
    fu->path = strdup(p);
    fu->hash = h;
    fu->func = func;
    fu->arg = arg;

    // 直接插入到头结点部分
    atom_lock(_s.lock);
    fu->next = _s.head;
    _s.head = fu;
    atom_unlock(_s.lock);
}

// files get 
static struct file * _get(const char * p, unsigned * r) {
    struct file * fu = _s.head;
    unsigned h = *r = str_hash(p);

    while (fu) {
        if (fu->hash == h && strcmp(fu->path, p) == 0)
            break;
        fu = fu->next;
    }

    return fu;
}

//
// file_set - 文件注册触发行为
// path     : 文件路径
// func     : file update -> func(path -> FILE, arg), func is NULL 标记清除
// arg      : 注入的额外参数
// return   : void
//
void 
file_set(const char * path, file_f func, void * arg) {
    unsigned h;
    assert(path && *path);
    struct file * fu = _get(path, &h);
    if (NULL == fu)
        _add(path, h, func, arg);
    else {
        atom_lock(_s.lock);
        fu->last = -1;
        fu->func = func;
        fu->arg = arg;
        atom_unlock(_s.lock);
    }
}

//
// file_update - 更新注册配置解析事件
// return   : void
//
void 
file_update(void) {
    struct file * fu;
    atom_lock(_s.lock);

    fu = _s.head;
    while (fu) {
        struct file * next = fu->next;

        if (NULL == fu->func) {
            // 删除的是头结点
            if (_s.head == fu)
                _s.head = next;

            free(fu->path);
            free(fu);
        } else {
            time_t last = mtime(fu->path);
            if (fu->last != last && last != -1) {
                FILE * c = fopen(fu->path, "rb+");
                if (NULL == c) {
                    CERR("fopen rb+ error = %s.", fu->path);
                    continue;
                }
                fu->last = last;
                fu->func(c, fu->arg);
                fclose(c);
            }
        }

        fu = next;
    }
    atom_unlock(_s.lock);
}
