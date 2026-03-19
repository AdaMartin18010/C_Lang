/*
 * Auto-generated from: 12_Practice_Exercises\02_Pointer_Exercises.md
 * Line: 85
 * Language: c
 * Block ID: f351116d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    void *ptr;
    size_t size;
    const char *file;
    int line;
} AllocInfo;

// 实现以下API:
void* safe_malloc(size_t size, const char *file, int line);
void* safe_calloc(size_t nmemb, size_t size, const char *file, int line);
void* safe_realloc(void *ptr, size_t size, const char *file, int line);
void safe_free(void *ptr);
void print_memory_leaks(void);

// 使用宏简化调用
#define SAFE_MALLOC(size) safe_malloc(size, __FILE__, __LINE__)
#define SAFE_FREE(ptr) safe_free(ptr)
