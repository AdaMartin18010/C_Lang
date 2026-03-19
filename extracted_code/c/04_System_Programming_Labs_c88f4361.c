/*
 * Auto-generated from: 12_Practice_Exercises\04_System_Programming_Labs.md
 * Line: 375
 * Language: c
 * Block ID: c88f4361
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 拦截malloc/free
#define malloc(size) my_malloc(size, __FILE__, __LINE__)
#define free(ptr) my_free(ptr)

// 记录分配信息
typedef struct AllocInfo {
    void *ptr;
    size_t size;
    char file[64];
    int line;
    time_t timestamp;
    struct AllocInfo *next;
} AllocInfo;

// 报告
void mem_report(void);
void mem_dump_leaks(void);
void mem_stats(size_t *total_alloc, size_t *total_free, size_t *current);
