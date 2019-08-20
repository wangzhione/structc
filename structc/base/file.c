﻿#include "file.h"

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
    struct file * list;     // 当前文件对象集
} f_s;

// files add 
static void f_s_add(const char * p, unsigned h, file_f func, void * arg) {
    struct file * fu;
    if (st_mtime(p) == -1) {
        RETNIL("mtime error p = %s", p);
    }

    fu = malloc(sizeof(struct file));
    fu->last = -1;
    fu->path = strdup(p);
    fu->hash = h;
    fu->func = func;
    fu->arg = arg;

    // 直接插入到头结点部分
    atom_lock(f_s.lock);
    fu->next = f_s.list;
    f_s.list = fu;
    atom_unlock(f_s.lock);
}

// files get 
static struct file * f_s_get(const char * p, unsigned * r) {
    struct file * fu = f_s.list;
    unsigned h = *r = str_hash(p);

    while (fu) {
        if (fu->hash == h && strcmp(fu->path, p) == 0)
            break;
        fu = fu->next;
    }

    return fu;
}

//
// file_set - 文件注册更新行为
// path     : 文件路径
// func     : NULL 标记清除, 正常 update -> func(path -> FILE, arg)
// arg      : func 额外参数
// return   : void
//
void 
file_set(const char * path, file_f func, void * arg) {
    unsigned h;
    assert(path && *path);
    struct file * fu = f_s_get(path, &h);
    if (!fu)
        f_s_add(path, h, func, arg);
    else {
        atom_lock(f_s.lock);
        fu->last = -1;
        fu->func = func;
        fu->arg = arg;
        atom_unlock(f_s.lock);
    }
}

//
// file_update - 配置文件刷新操作
// return   : void
//
void 
file_update(void) {
    atom_lock(f_s.lock);
    struct file * fu = f_s.list;
    while (fu) {
        struct file * next = fu->next;

        if (!fu->func) {
            // 删除的是头结点
            if (f_s.list == fu)
                f_s.list = next;

            free(fu->path);
            free(fu);
        } else {
            time_t last = st_mtime(fu->path);
            if (fu->last != last && last != -1) {
                FILE * c = fopen(fu->path, "rb+");
                if (!c) {
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
    atom_unlock(f_s.lock);
}
