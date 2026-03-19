/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 816
 * Language: c
 * Block ID: f04f934a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 模式2: 数组分配（三种方式对比）
 */

// 方式A: malloc + 手动计算大小
int *alloc_array_malloc(size_t n) {
    int *arr = malloc(n * sizeof(int));  // 风险: n * sizeof 可能溢出
    return arr;
}

// 方式B: calloc（推荐用于数组）
int *alloc_array_calloc(size_t n) {
    int *arr = calloc(n, sizeof(int));   // 自动计算大小并清零
    return arr;
}

// 方式C: reallocarray（BSD/GNU扩展，溢出安全）
int *alloc_array_safe(size_t n) {
#ifdef __GLIBC__
    int *arr = reallocarray(NULL, n, sizeof(int));  // 检查溢出
#else
    if (n > SIZE_MAX / sizeof(int)) {  // 手动溢出检查
        return NULL;
    }
    int *arr = calloc(n, sizeof(int));
#endif
    return arr;
}

/*
 * 模式3: 柔性数组(Flexible Array Member)
 */
typedef struct {
    size_t count;
    int data[];  // C99柔性数组，必须在最后
} IntVector;

IntVector *create_vector(size_t n) {
    // 分配: 结构体大小 + 数组大小
    IntVector *v = malloc(sizeof(IntVector) + n * sizeof(int));
    if (v == NULL) return NULL;

    v->count = n;
    for (size_t i = 0; i < n; i++) {
        v->data[i] = 0;
    }
    return v;
}
