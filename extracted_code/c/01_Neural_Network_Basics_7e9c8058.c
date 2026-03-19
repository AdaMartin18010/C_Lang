/*
 * Auto-generated from: 11_Machine_Learning_C\01_Neural_Network_Basics.md
 * Line: 2434
 * Language: c
 * Block ID: 7e9c8058
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ==================== 常见内存错误与防范 ====================

/*
 * 错误1: 忘记释放内存，导致内存泄漏
 * 防范: 使用RAII模式，确保每个malloc都有对应的free
 */

// 良好的内存管理模式
int safe_training_example() {
    double* buffer = NULL;
    NeuralNetwork* nn = NULL;
    int result = 0;

    // 分配
    buffer = malloc(1024 * sizeof(double));
    if (!buffer) { result = -1; goto cleanup; }

    nn = nn_create(...);
    if (!nn) { result = -1; goto cleanup; }

    // 使用...

cleanup:
    // 统一清理
    free(buffer);
    if (nn) nn_free(nn);
    return result;
}

/*
 * 错误2: 使用已释放的内存 (use-after-free)
 * 防范: 释放后立即置NULL
 */
void safe_free(void** ptr) {
    if (*ptr) {
        free(*ptr);
        *ptr = NULL;
    }
}

/*
 * 错误3: 缓冲区溢出
 * 防范: 始终使用边界检查
 */
void safe_matrix_set(Matrix* m, int i, int j, double val) {
    if (i < 0 || i >= m->rows || j < 0 || j >= m->cols) {
        fprintf(stderr, "Error: Index out of bounds (%d, %d) for matrix [%d x %d]\n",
                i, j, m->rows, m->cols);
        return;
    }
    m->data[i * m->cols + j] = val;
}

/*
 * 错误4: 内存对齐问题（SIMD要求）
 * 防范: 使用对齐分配函数
 */
#ifdef __AVX__
void* aligned_alloc_avx(size_t size) {
    void* ptr = NULL;
    #ifdef _WIN32
    ptr = _aligned_malloc(size, 32);  // AVX需要32字节对齐
    #else
    posix_memalign(&ptr, 32, size);
    #endif
    return ptr;
}
#endif

/*
 * 错误5: 栈溢出（大数组分配在栈上）
 * 防范: 大数组使用堆分配
 */
// 错误: 栈溢出
void bad_function() {
    double huge_array[1000000];  // 8MB，可能超出栈限制
}

// 正确: 堆分配
void good_function() {
    double* huge_array = malloc(1000000 * sizeof(double));
    // 使用...
    free(huge_array);
}

// 内存调试辅助函数
#ifdef DEBUG_MEMORY
static size_t total_allocated = 0;
static size_t total_freed = 0;

void* debug_malloc(size_t size, const char* file, int line) {
    void* ptr = malloc(size);
    total_allocated += size;
    printf("[ALLOC] %p size=%zu at %s:%d (total: %zu)\n",
           ptr, size, file, line, total_allocated);
    return ptr;
}

void debug_free(void* ptr, size_t size, const char* file, int line) {
    total_freed += size;
    printf("[FREE]  %p size=%zu at %s:%d (total: %zu)\n",
           ptr, size, file, line, total_allocated - total_freed);
    free(ptr);
}

#define malloc(size) debug_malloc(size, __FILE__, __LINE__)
#define free(ptr) debug_free(ptr, sizeof(*(ptr)), __FILE__, __LINE__)
#endif
