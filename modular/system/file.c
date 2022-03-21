#include "file.h"

struct file {
    time_t last;            // 文件最后修改时间点
    file_f func;            // 执行行为
    void * arg;             // 行为参数
};

static struct file * file_create(const char * path, file_f func, void * arg) {
    assert(path && func);

    if (fmtime(path) == -1) {
        RETURN(NULL, "mtime error p = %s", path);
    }

    struct file * fu = malloc(sizeof(struct file));
    if (NULL == fu) {
        return NULL;
    }

    fu->last = -1;
    fu->func = func;
    fu->arg = arg;

    return fu;
}

static inline void file_delete(struct file * fu) {
    free(fu);
}

struct files {
    atomic_flag data_lock;
    // const char * path key -> value struct file
    // 用于 update 数据
    volatile dict_t data;

    atomic_flag backup_lock;
    // const char * path key -> value struct file
    // 在 update 兜底备份数据
    volatile dict_t backup;
};

static struct files F = {
    .data_lock = ATOMIC_FLAG_INIT,
    .backup_lock = ATOMIC_FLAG_INIT,
};

extern void file_init() {
    F.data = dict_create(file_delete);
    F.backup = dict_create(file_delete);
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
    struct file * fu = NULL;
    assert(path && *path);

    // step 1 : 尝试竞争 data lock
    if (atomic_flag_trylock(&F.data_lock)) {
        if (NULL != func) {
            fu = file_create(path, func, arg);
        }
        dict_set(F.data, path, fu);
        return atomic_flag_unlock(&F.data_lock);
    }

    // step 2 : data lock 没有竞争到, 直接竞争 backup lock
    atomic_flag_lock(&F.backup_lock);
    fu = file_create(path, func, arg);
    dict_set(F.backup, path, fu);
    atomic_flag_unlock(&F.backup_lock);
}

static int file_filter(struct file * fu) {
    return fu->func == NULL ? 0 : 1;
}

static int file_each(const char * path, struct file * fu, void * arg) {
    // 更新操作
    time_t last = fmtime(path);
    if (fu->last != last && last != -1) {
        FILE * c = fopen(path, "rb+");
        if (NULL == c) {
            PERR("fopen error rb+ %s.", path);
            return -1;
        }
        fu->last = last;
        fu->func(c, fu->arg);
        fclose(c);
    }

    (void)arg;
    return 0;
}

//
// file_update - 配置文件刷新操作
// return   : void
//
void 
file_update(void) {
    // step 0 : 抢占 data lock
    atomic_flag_lock(&F.data_lock);

    // step 1 : backup move data
    atomic_flag_lock(&F.backup_lock);
    dict_move_filter(F.data, F.backup, file_filter);
    atomic_flag_unlock(&F.backup_lock);

    // 尝试更新操作
    dict_each(F.data, file_each, NULL);

    atomic_flag_unlock(&F.data_lock);
}

/*
 C 复杂的数据结构能力缺乏灵活性. 往往业务代码和底层代码互相交融. 很多时候需要深度定制化.
 */
