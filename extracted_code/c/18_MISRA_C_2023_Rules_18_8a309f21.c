/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\18_MISRA_C_2023_Rules_18.md
 * Line: 313
 * Language: c
 * Block ID: 8a309f21
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 固定大小 */
#define MAX_SIZE 1000
void process_fixed(void)
{
    int arr[MAX_SIZE];  /* 固定大小 */
}

/* ✅ 合规 - 动态分配 */
void process_dynamic(int n)
{
    int *arr = malloc(n * sizeof(int));
    if (arr == NULL) {
        /* 错误处理 */
        return;
    }
    /* 使用arr */
    free(arr);
}

/* ✅ 合规 - 内存池 */
void *pool_alloc(size_t size);
void pool_free(void *ptr);

void process_pool(int n)
{
    int *arr = pool_alloc(n * sizeof(int));
    if (arr == NULL) {
        return;
    }
    /* 使用arr */
    pool_free(arr);
}

/* ✅ 合规 - 柔性数组 */
struct Buffer {
    size_t size;
    uint8_t data[];
};

struct Buffer *create_buffer(size_t size)
{
    struct Buffer *b = malloc(sizeof(struct Buffer) + size);
    if (b) {
        b->size = size;
    }
    return b;
}
