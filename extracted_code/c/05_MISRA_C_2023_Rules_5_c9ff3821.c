/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\05_MISRA_C_2023_Rules_5.md
 * Line: 441
 * Language: c
 * Block ID: c9ff3821
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 使用固定大小 */
#define MAX_SIZE 100
void process(void)
{
    int arr[MAX_SIZE];  /* 固定大小 */
}

/* ✅ 合规 - 使用动态分配 */
void process(int n)
{
    int *arr = malloc(n * sizeof(int));  /* 堆分配 */
    if (arr == NULL) {
        /* 处理错误 */
        return;
    }
    /* 使用arr */
    free(arr);
}

/* ✅ 合规 - 使用柔性数组（C99）*/
struct buffer {
    size_t size;
    int data[];  /* 柔性数组成员 */
};

struct buffer *create_buffer(size_t n)
{
    struct buffer *b = malloc(sizeof(struct buffer) + n * sizeof(int));
    if (b) {
        b->size = n;
    }
    return b;
}
