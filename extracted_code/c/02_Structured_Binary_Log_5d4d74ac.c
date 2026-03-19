/*
 * Auto-generated from: 03_System_Technology_Domains\09_High_Performance_Log\02_Structured_Binary_Log.md
 * Line: 433
 * Language: c
 * Block ID: 5d4d74ac
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* MMAP日志写入器 */
typedef struct {
    int fd;
    char *filename;

    uint8_t *mmap_base;
    size_t mmap_size;
    size_t page_size;

    /* 写入位置 */
    size_t write_pos;
    size_t committed_pos;

    /* 滚动配置 */
    size_t max_file_size;
    uint32_t max_files;
} MmapLogWriter;

/* 创建mmap写入器 */
MmapLogWriter* mmap_writer_create(const char *path, size_t max_size) {
    MmapLogWriter *w = calloc(1, sizeof(MmapLogWriter));

    w->fd = open(path, O_RDWR | O_CREAT, 0644);
    if (w->fd < 0) goto error;

    w->page_size = sysconf(_SC_PAGESIZE);
    w->max_file_size = max_size;

    /* 预分配文件大小 */
    if (ftruncate(w->fd, w->page_size) < 0) goto error;

    /* 初始mmap */
    w->mmap_size = w->page_size;
    w->mmap_base = mmap(NULL, w->mmap_size, PROT_READ | PROT_WRITE,
                        MAP_SHARED, w->fd, 0);
    if (w->mmap_base == MAP_FAILED) goto error;

    return w;

error:
    free(w);
    return NULL;
}

/* 写入数据 */
int mmap_writer_append(MmapLogWriter *w, const void *data, size_t len) {
    /* 检查是否需要扩容 */
    if (w->write_pos + len > w->mmap_size) {
        /* 扩展mmap */
        size_t new_size = w->mmap_size * 2;
        while (new_size < w->write_pos + len) {
            new_size *= 2;
        }

        /* 取消当前映射 */
        msync(w->mmap_base, w->write_pos, MS_SYNC);
        munmap(w->mmap_base, w->mmap_size);

        /* 扩展文件 */
        if (ftruncate(w->fd, new_size) < 0) return -1;

        /* 重新映射 */
        w->mmap_base = mmap(NULL, new_size, PROT_READ | PROT_WRITE,
                           MAP_SHARED, w->fd, 0);
        if (w->mmap_base == MAP_FAILED) return -1;

        w->mmap_size = new_size;
    }

    /* 写入数据 */
    memcpy(w->mmap_base + w->write_pos, data, len);
    w->write_pos += len;

    return 0;
}

/* 强制刷盘 */
void mmap_writer_sync(MmapLogWriter *w, bool async) {
    int flags = async ? MS_ASYNC : MS_SYNC;
    msync(w->mmap_base, w->write_pos, flags);
    w->committed_pos = w->write_pos;
}
