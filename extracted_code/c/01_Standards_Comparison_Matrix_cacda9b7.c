/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\01_Standards_Comparison_Matrix.md
 * Line: 995
 * Language: c
 * Block ID: cacda9b7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 方案1：使用alloca()（GNU扩展） */
void use_alloca(int n) {
    int* arr = alloca(n * sizeof(int));
    /* ... */
}

/* 方案2：使用柔性数组成员 */
typedef struct {
    size_t len;
    int data[];  /* FAM */
} FlexArray;

/* 方案3：使用malloc（推荐） */
void use_malloc(int n) {
    int* arr = malloc(n * sizeof(int));
    if (arr) {
        /* ... */
        free(arr);
    }
}

/* 方案4：C23 constexpr + 固定最大尺寸 */
constexpr int MAX_SIZE = 10000;
void use_fixed_max(int n) {
    if (n > MAX_SIZE) {
        /* 错误处理 */
        return;
    }
    int arr[MAX_SIZE];  /* 固定大小，只使用前n个元素 */
    /* ... */
}
