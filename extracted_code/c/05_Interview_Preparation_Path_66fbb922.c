/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\05_Interview_Preparation_Path.md
 * Line: 427
 * Language: c
 * Block ID: 66fbb922
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 1. 工具检测
// Valgrind: valgrind --leak-check=full --show-leak-kinds=all ./program
// AddressSanitizer: gcc -fsanitize=address -g

// 2. 自定义检测（Hook malloc/free）
#define _GNU_SOURCE
#include <dlfcn.h>

typedef struct AllocInfo {
    void *ptr;
    size_t size;
    const char *file;
    int line;
    struct AllocInfo *next;
} AllocInfo;

static AllocInfo *alloc_list = NULL;
static pthread_mutex_t alloc_lock = PTHREAD_MUTEX_INITIALIZER;

void *malloc(size_t size) {
    static void *(*real_malloc)(size_t) = NULL;
    if (!real_malloc) real_malloc = dlsym(RTLD_NEXT, "malloc");

    void *p = real_malloc(size);

    // 记录分配信息
    AllocInfo *info = real_malloc(sizeof(AllocInfo));
    info->ptr = p;
    info->size = size;
    info->next = alloc_list;

    pthread_mutex_lock(&alloc_lock);
    alloc_list = info;
    pthread_mutex_unlock(&alloc_lock);

    return p;
}

void print_leaks() {
    pthread_mutex_lock(&alloc_lock);
    AllocInfo *p = alloc_list;
    while (p) {
        printf("Leak: %p, size %zu\n", p->ptr, p->size);
        p = p->next;
    }
    pthread_mutex_unlock(&alloc_lock);
}
