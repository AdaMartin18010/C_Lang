/*
 * Auto-generated from: 03_System_Technology_Domains\09_High_Performance_Log\02_Structured_Binary_Log.md
 * Line: 520
 * Language: c
 * Block ID: 9272814c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 日志滚动管理 */
typedef struct {
    char *base_path;
    uint32_t current_file;
    uint32_t max_files;

    MmapLogWriter *current;

    /* 索引 */
    struct {
        uint64_t *timestamps;
        uint64_t *file_offsets;
        uint32_t count;
    } index;
} RollingLog;

/* 检查并执行滚动 */
bool rolling_log_check_rotate(RollingLog *rl) {
    if (rl->current->write_pos < rl->current->max_file_size) {
        return false;  /* 不需要滚动 */
    }

    /* 关闭当前文件 */
    mmap_writer_sync(rl->current, false);
    mmap_writer_close(rl->current);

    /* 生成新文件名 */
    char path[256];
    rl->current_file++;
    snprintf(path, sizeof(path), "%s.%08u.log",
             rl->base_path, rl->current_file);

    /* 创建新写入器 */
    rl->current = mmap_writer_create(path, rl->current->max_file_size);

    /* 清理旧文件 */
    if (rl->current_file >= rl->max_files) {
        char old_path[256];
        snprintf(old_path, sizeof(old_path), "%s.%08u.log",
                 rl->base_path, rl->current_file - rl->max_files);
        unlink(old_path);
    }

    return true;
}
