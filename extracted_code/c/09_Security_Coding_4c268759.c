/*
 * Auto-generated from: 12_Practice_Exercises\09_Security_Coding.md
 * Line: 61
 * Language: c
 * Block ID: 4c268759
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    uint64_t canary_before;
    size_t size;
    uint32_t flags;
    uint64_t canary_after;
    // 用户数据从这里开始
} BlockHeader;

#define CANARY_VALUE 0xDEADBEEFCAFEBABEULL

void* safe_malloc(size_t size);
void* safe_calloc(size_t nmemb, size_t size);
void* safe_realloc(void *ptr, size_t size);
void safe_free(void *ptr);

// 检查堆完整性
bool heap_check(void);

// 调试功能
void heap_dump_leaks(void);
size_t heap_total_allocated(void);
