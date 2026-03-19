/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\03_Qsort_Branch_Prediction.md
 * Line: 652
 * Language: c
 * Block ID: 330e0c2f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * glibc qsort实现要点：
 *
 * 1. 混合算法：
 *    - 主要使用内排（快速排序 + 堆排序）
 *    - 小数组使用插入排序
 *    - 大数组使用归并排序（稳定版本）
 *
 * 2. 优化技术：
 *    - 尾递归消除
 *    - 栈深度限制
 *    - 间接排序（减少交换成本）
 */

// glibc风格的比较函数包装
// 减少函数调用开销

typedef struct {
    const void *key;
    void *obj;
} QsortElement;

// 间接排序：排序索引而非数据
typedef struct {
    const void *base;
    size_t size;
    int (*cmp)(const void *, const void *);
} QsortContext;

int indirect_compare(const void *a, const void *b, void *arg) {
    QsortContext *ctx = arg;
    size_t idx_a = *(const size_t*)a;
    size_t idx_b = *(const size_t*)b;

    return ctx->cmp(
        (const char*)ctx->base + idx_a * ctx->size,
        (const char*)ctx->base + idx_b * ctx->size
    );
}

// 稳定排序（使用归并）
void qsort_stable(void *base, size_t n, size_t size,
                  int (*cmp)(const void *, const void *)) {
    if (n <= 1) return;

    // 创建索引数组
    size_t *indices = malloc(n * sizeof(size_t));
    for (size_t i = 0; i < n; i++) {
        indices[i] = i;
    }

    // 排序索引
    QsortContext ctx = {base, size, cmp};
    qsort_r(indices, n, sizeof(size_t), indirect_compare, &ctx);

    // 原地重排（循环节算法）
    char *temp = malloc(size);
    char *arr = base;

    for (size_t i = 0; i < n; i++) {
        size_t j = i;
        while (indices[j] != i) {
            // 交换arr[i]和arr[indices[j]]
            memcpy(temp, arr + i * size, size);
            memcpy(arr + i * size, arr + indices[j] * size, size);
            memcpy(arr + indices[j] * size, temp, size);

            size_t next_j = indices[j];
            indices[j] = j;
            j = next_j;
        }
        indices[j] = j;
    }

    free(temp);
    free(indices);
}
