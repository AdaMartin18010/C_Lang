/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\07_MISRA_C_2023_Rules_7.md
 * Line: 554
 * Language: c
 * Block ID: 2db9f970
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 内存所有权模式 */

/* 1. 谁分配，谁释放 */
void caller_owned(void)
{
    int *p = malloc(100);  /* 分配 */
    use_buffer(p);
    free(p);  /* 释放 */
}

/* 2. 工厂模式 */
struct object *create_object(void);  /* 创建并返回所有权 */
void destroy_object(struct object *obj);  /* 接受并销毁 */

/* 3. 借用模式（不转移所有权）*/
void use_buffer(const char *data);  /* 只读借用 */
void modify_buffer(char *data, size_t len);  /* 可写借用 */

/* 内存泄漏检测（调试）*/
#ifdef DEBUG_MEMORY
#include <stdlib.h>

static size_t alloc_count = 0;
static size_t free_count = 0;

void *debug_malloc(size_t size, const char *file, int line)
{
    void *p = malloc(size);
    if (p != NULL) {
        alloc_count++;
        printf("[ALLOC] %p at %s:%d\n", p, file, line);
    }
    return p;
}

void debug_free(void *p, const char *file, int line)
{
    if (p != NULL) {
        free_count++;
        printf("[FREE] %p at %s:%d\n", p, file, line);
    }
    free(p);
}

#define malloc(s) debug_malloc(s, __FILE__, __LINE__)
#define free(p) debug_free(p, __FILE__, __LINE__)

#endif
