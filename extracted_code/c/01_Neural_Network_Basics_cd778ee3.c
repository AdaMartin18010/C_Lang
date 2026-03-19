/*
 * Auto-generated from: 11_Machine_Learning_C\01_Neural_Network_Basics.md
 * Line: 1860
 * Language: c
 * Block ID: cd778ee3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * SoA (Structure of Arrays) vs AoS (Array of Structures)
 * 神经网络参数优化: 使用连续内存布局提高缓存效率
 */

// 原始AoS布局（不推荐用于大型网络）
typedef struct {
    double value;
    double grad;
} Parameter_AoS;

// 推荐SoA布局
typedef struct {
    double* values;  // 连续存储
    double* grads;   // 连续存储
    int size;
} Parameter_SoA;

// 层参数的连续内存布局
typedef struct {
    // 权重: [n_outputs x n_inputs] 连续存储
    double* W;
    double* dW;

    // 偏置: [n_outputs] 连续存储
    double* b;
    double* db;

    int n_inputs;
    int n_outputs;

    // 使用stride访问，支持对齐
    int stride;  // 通常为n_inputs的向上取整到64字节边界
} OptimizedLayer;

// 分配对齐内存（64字节边界，适配缓存行）
void* aligned_malloc(size_t size, size_t alignment) {
    void* ptr;
    #ifdef _WIN32
    ptr = _aligned_malloc(size, alignment);
    #else
    if (posix_memalign(&ptr, alignment, size) != 0) ptr = NULL;
    #endif
    return ptr;
}

void aligned_free(void* ptr) {
    #ifdef _WIN32
    _aligned_free(ptr);
    #else
    free(ptr);
    #endif
}

// 创建优化的层
OptimizedLayer* optimized_layer_create(int n_inputs, int n_outputs) {
    OptimizedLayer* layer = malloc(sizeof(OptimizedLayer));
    layer->n_inputs = n_inputs;
    layer->n_outputs = n_outputs;

    // stride对齐到64字节（8个double）
    layer->stride = ((n_inputs + 7) / 8) * 8;

    size_t weight_size = n_outputs * layer->stride * sizeof(double);
    layer->W = aligned_malloc(weight_size, 64);
    layer->dW = aligned_malloc(weight_size, 64);
    layer->b = aligned_malloc(n_outputs * sizeof(double), 64);
    layer->db = aligned_malloc(n_outputs * sizeof(double), 64);

    // 初始化
    memset(layer->W, 0, weight_size);
    memset(layer->dW, 0, weight_size);
    memset(layer->b, 0, n_outputs * sizeof(double));
    memset(layer->db, 0, n_outputs * sizeof(double));

    return layer;
}

// 使用stride访问权重
static inline double get_weight(const OptimizedLayer* layer, int i, int j) {
    return layer->W[i * layer->stride + j];
}

static inline void set_weight(OptimizedLayer* layer, int i, int j, double val) {
    layer->W[i * layer->stride + j] = val;
}
