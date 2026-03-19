/*
 * Auto-generated from: 03_System_Technology_Domains\09_High_Performance_Log\02_Structured_Binary_Log.md
 * Line: 574
 * Language: c
 * Block ID: b84d4c73
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 时间范围查询 */
typedef struct {
    uint64_t start_time;
    uint64_t end_time;
    uint32_t min_level;
    const char *pattern;  /* 可选的消息模式 */
} LogQuery;

/* 二分查找定位起始位置 */
size_t log_index_find(const RollingLog *rl, uint64_t timestamp) {
    int left = 0;
    int right = rl->index.count - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (rl->index.timestamps[mid] < timestamp) {
            left = mid + 1;
        } else if (rl->index.timestamps[mid] > timestamp) {
            right = mid - 1;
        } else {
            return mid;
        }
    }

    return left;  /* 返回第一个>=timestamp的位置 */
}

/* 范围查询 */
int log_query_range(RollingLog *rl, const LogQuery *q,
                    LogEntry *results, uint32_t max_results) {
    /* 定位起始文件 */
    size_t start_idx = log_index_find(rl, q->start_time);
    uint32_t found = 0;

    for (size_t i = start_idx; i < rl->index.count && found < max_results; i++) {
        /* 打开对应文件 */
        char path[256];
        snprintf(path, sizeof(path), "%s.%08u.log",
                 rl->base_path, (uint32_t)(rl->index.file_offsets[i] >> 32));

        LogReader *r = log_reader_open(path);
        if (!r) continue;

        /* 跳转到偏移位置 */
        r->read_pos = rl->index.file_offsets[i] & 0xFFFFFFFF;
        r->buf_pos = r->read_pos;

        /* 读取条目 */
        LogEntry entry;
        while (log_reader_next(r, &entry) && found < max_results) {
            if (entry.payload.timestamp_us > q->end_time) {
                break;
            }

            if (entry.payload.level < q->min_level) {
                continue;
            }

            if (q->pattern && !strstr(entry.payload.message, q->pattern)) {
                continue;
            }

            results[found++] = entry;
        }

        log_reader_close(r);
    }

    return found;
}
