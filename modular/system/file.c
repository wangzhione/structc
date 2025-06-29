#include "file.h"

struct file {
    file_f func;        // 执行行为, nullptr 标识删除
    void * arg;         // 行为参数
    char * path;        // 文件路径
    unsigned hash;      // path hash
    time_t lasttime;    // 文件最后修改时间点

    struct file * next;
};

static struct file * file_create(unsigned hash, const char * path, file_f func, void * arg) {
    assert(path && func);
    
    if (fmtime(path) == -1) {
        RETURN(nullptr, "mtime error p = %s", path);
    }

    struct file * fu = malloc(sizeof(struct file));
    if (nullptr == fu) {
        return nullptr;
    }
    fu->path = strdup(path);
    if (nullptr == path) {
        free(fu);
        return nullptr;
    }

    fu->lasttime = -1;
    fu->func = func;
    fu->arg = arg;
    fu->hash = hash;
    fu->next = nullptr;
    return fu;
}

static inline void file_delete(struct file * fu) {
    free(fu->path);
    free(fu);
}

static int file_each(struct file * fu) {
    char * path = fu->path;
    // 更新操作
    time_t lasttime = fmtime(path);
    if (lasttime == -1) {
        RETURN(-1, "fmtime error path = %s", path);
    }

    if (fu->lasttime != lasttime) {
        FILE * c = fopen(path, "rb+");
        if (c == nullptr) {
            RETURN(-1, "fopen error rb+ %s", path);
        }
        fu->lasttime = lasttime;
        fu->func(c, fu->arg);
        fclose(c);
    }

    return 0;
}

struct files {
    atomic_flag data_lock;
    // 用于 update 数据 empty head
    struct file data;

    atomic_flag backup_lock;
    // 在 update 兜底备份数据 empty head
    struct file backup;
};

static struct files F = {
    .data_lock = ATOMIC_FLAG_INIT,
    .backup_lock = ATOMIC_FLAG_INIT,
};

static void files_replace(const char * path, file_f func, void * arg) {
    unsigned hash = BKDHash(path);
    struct file * prev = &F.data;
    struct file * node = prev->next;
    // list struct 瓶颈所在 find O(n)
    while (node != nullptr) {
        if (node->hash == hash && strcmp(node->path, path) == 0) {
            break;
        }
        prev = node;
        node = node->next;
    }
    // node != nullptr 标识找到这个结点
    if (node != nullptr) {
        if (func == nullptr) {
            prev->next = node->next;
            file_delete(node);
        } else {
            node->func = func;
            node->arg = arg;
        }
        return;
    }
    // node == nullptr 标识没有找到这个结点
    if (func == nullptr) {
        prev->next = file_create(hash, path, func, arg);
    }
}

static void files_add(const char * path, file_f func, void * arg) {
    unsigned hash = BKDHash(path);
    struct file * prev = &F.backup;
    struct file * node = prev->next;
    while (node != nullptr) {
        if (node->hash == hash && strcmp(node->path, path) == 0) {
            break;
        }
        prev = node;
        node = node->next;
    }
    // node == nullptr 标识没有找到这个结点, 触发添加
    if (node == nullptr) {
        prev->next = file_create(hash, path, func, arg);
        return;
    }
    // node != nullptr 标识找到这个结点, 触发更新
    node->func = func;
    node->arg = arg;
}

static void files_move_data(struct file * backup_node) {
    struct file * data_prev = &F.data;
    struct file * data_node = data_prev->next;
    while (data_node != nullptr) {
        if (data_node->hash == backup_node->hash 
            && strcmp(data_node->path, backup_node->path) == 0) {
            break;
        }
        data_prev = data_node;
        data_node = data_node->next;
    }
    // data_node == nullptr 标识没有找到这个结点
    if (data_node == nullptr) {
        if (backup_node->func == nullptr) 
            file_delete(backup_node);
        else {
            // 从 backup 链表中摘出去
            backup_node->next = nullptr;
            // 触发添加操作
            data_prev->next = backup_node;
        }
        return;
    }
    // data_node != nullptr 标识找到这个结点
    if (backup_node->func == nullptr) {
        // 触发删除操作
        data_prev->next = data_node->next;
        file_delete(data_node);
    } else {
        // 触发更新操作
        data_node->func = backup_node->func;
        data_node->arg = backup_node->arg;
    }
    file_delete(backup_node);
}

// backup move data
static void files_move(void) {
    struct file * backup_prev = &F.backup;
    struct file * backup_node = backup_prev->next;
    while (backup_node != nullptr) {
        struct file * next = backup_node->next;
        // 尝试在移动到 data 中, 并维护好结点关系
        files_move_data(backup_node);
        backup_node = next;
    }
    // 重置 backup 链表
    F.backup.next = nullptr;
}

//
// file_set - 文件注册更新行为
// path     : 文件路径
// func     : nullptr 标识清除, 正常 update -> func(path -> FILE, arg)
// arg      : func 额外参数
// return   : void
//
void 
file_set(const char * path, file_f func, void * arg) {
    assert(path && *path);

    // step 1 : 尝试竞争 data lock
    if (atomic_flag_trylock(&F.data_lock)) {
        files_replace(path, func, arg);
        return atomic_flag_unlock(&F.data_lock);
    }

    // step 2 : data lock 没有竞争到, 直接竞争 backup lock
    atomic_flag_lock(&F.backup_lock);
    files_add(path, func, arg);
    atomic_flag_unlock(&F.backup_lock);
}

//
// file_update - 配置文件刷新操作
// return   : void
//
void 
file_update(void) {
    // step 1 : 抢占 data lock
    atomic_flag_lock(&F.data_lock);

    // step 2 : backup move data
    atomic_flag_lock(&F.backup_lock);
    files_move();
    atomic_flag_unlock(&F.backup_lock);

    // step 3 : 尝试更新操作
    struct file * prev = &F.data;
    struct file * node = prev->next;
    // list struct 瓶颈所在 find O(n)
    while (node != nullptr) {
        if (node->func != nullptr) {
            int res = file_each(node);
            POUT("file_each res = %d, path = %s", res, node->path);
            prev = node;
            node = node->next;
        } else {
            prev->next = node->next;
            file_delete(node);
            node = prev->next;
        }
    }

    atomic_flag_unlock(&F.data_lock);
}

/*
 C 复杂的数据结构能力缺乏灵活性. 往往业务代码和底层代码互相交融. 很多时候需要深度定制化.
 */
