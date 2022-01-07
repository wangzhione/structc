#include "file.h"

struct file {
    time_t last;            // 文件最后修改时间点
    char * path;            // 文件全路径
    unsigned hash;          // 文件路径 hash 值

    file_f func;            // 执行行为
    void * arg;             // 行为参数

    struct file * next;     // 文件下一个结点
};

static struct file * file_create(const char * path, unsigned h, file_f func, void * arg) {
    assert(path && func);

    if (fmtime(path) == -1) {
        RETURN(NULL, "mtime error p = %s", path);
    }

    struct file * fu = malloc(sizeof(struct file));
    if (NULL == fu) {
        return NULL;
    }

    fu->last = -1;
    fu->path = strdup(path);
    if (NULL == fu->path) {
        free(fu);
        return NULL;
    }

    fu->hash = h;
    fu->func = func;
    fu->arg = arg;

    // fu->next = NULL;

    return fu;
}

inline void file_delete(struct file * fu) {
    free(fu->path);
    free(fu);
}

static struct files {
    struct file * list;     // 当前文件对象集
} f_s;

// files add 
static void f_s_add(const char * path, unsigned hash, file_f func, void * arg) {
    struct file * fu = file_create(path, hash, func, arg);
    if (fu == NULL) {
        return;
    }

    // 直接插入到头结点部分
    fu->next = f_s.list;
    f_s.list = fu;
}

//
// file_set - 文件注册更新行为
// path     : 文件路径
// func     : NULL 标识清除, 正常 update -> func(path -> FILE, arg)
// arg      : func 额外参数
// return   : void
//
void 
file_set(const char * path, file_f func, void * arg) {
    assert(path && *path);

    unsigned hash = BKDHash(path);
    struct file * curr, * next;
    struct file * prev = NULL;

    for (curr = f_s.list; curr; curr = next) {
        next = curr->next;

        // 数据不匹配直接跳过
        if (curr->hash != hash || strcmp(curr->path, path) != 0) {
            continue;
        }

        // 找到了 path 相关结点
        if (NULL == func) {
            // 尝试删除结点
            if (NULL == prev) {
                // 删除的是头结点
                f_s.list = next;
            } else {
                prev->next = next;
            }

            file_delete(curr);
        } else {
            // 走更新操作
            curr->last = -1;
            curr->func = func;
            curr->arg = arg;
        }

        return;
    }

    // 没有找到, 我们走插入操作
    if (NULL != func) {
        f_s_add(path, hash, func, arg);
    }
}

//
// file_update - 配置文件刷新操作
// return   : void
//
void 
file_update(void) {
    struct file * prev = NULL;
    struct file * curr, * next;
    for (curr = f_s.list; curr; curr = next) {
        next = curr->next;

        // 删除清理操作 | 向后兼容
        if (NULL == curr->func) {
            if (NULL == prev) {
                // 删除的是头结点
                f_s.list = next;
            } else {
                prev->next = next;
            }

            file_delete(curr);
            continue;
        }

        // 更新操作
        time_t last = fmtime(curr->path);
        if (curr->last != last && last != -1) {
            FILE * c = fopen(curr->path, "rb+");
            if (!c) {
                PERR("fopen %s rb+ error.", curr->path);
                continue;
            }
            curr->last = last;
            curr->func(c, curr->arg);
            fclose(c);
        }
    }
}
