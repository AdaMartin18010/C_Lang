/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\02_Resource_Types.md
 * Line: 458
 * Language: c
 * Block ID: 1ec35aa6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 类型状态：通过类型编码状态机

typedef struct {
    int fd;
} FileClosed;

typedef struct {
    int fd;
} FileOpen;

typedef struct {
    int fd;
} FileReading;

// 状态转移函数
FileOpen *file_open(const char *path) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) return NULL;
    FileOpen *f = malloc(sizeof(FileOpen));
    f->fd = fd;
    return f;
}

FileReading *file_start_read(FileOpen *f) {
    FileReading *r = malloc(sizeof(FileReading));
    r->fd = f->fd;
    free(f);  // 状态转移，旧类型不可用
    return r;
}

void file_read(FileReading *f, void *buf, size_t len) {
    read(f->fd, buf, len);
}

FileOpen *file_stop_read(FileReading *f) {
    FileOpen *o = malloc(sizeof(FileOpen));
    o->fd = f->fd;
    free(f);
    return o;
}

void file_close(FileOpen *f) {
    close(f->fd);
    free(f);
}

// 编译时状态检查（使用不同结构体类型）
// file_read(closed_file, ...);  // 编译错误！
