/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\03_Hash_Table_Implementation.md
 * Line: 1224
 * Language: c
 * Block ID: 400384be
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 哈希表优化技巧集合
 */

// 技巧1: 预分配空间，避免频繁扩容
void hash_table_reserve(OpenAddressHashTable* table, size_t expected_size) {
    size_t new_capacity = (size_t)(expected_size / OA_MAX_LOAD_FACTOR) + 1;
    // 向上取整到2的幂次
    size_t power_of_2 = 1;
    while (power_of_2 < new_capacity) {
        power_of_2 <<= 1;
    }
    oa_resize(table, power_of_2);
}

// 技巧2: 批量插入 (比逐个插入更高效)
void hash_table_batch_insert(OpenAddressHashTable* table,
                              const int* keys, const int* values, size_t n) {
    // 预估所需容量
    size_t needed = (size_t)((table->size + n) / OA_MAX_LOAD_FACTOR) + 1;
    if (needed > table->capacity) {
        oa_resize(table, needed);
    }

    // 批量插入
    for (size_t i = 0; i < n; i++) {
        oa_hash_insert(table, keys[i], values[i]);
    }
}

// 技巧3: 使用开放寻址法的变体 - 罗宾汉哈希
// 减少探测序列长度的方差

typedef struct {
    int key;
    int value;
    uint32_t hash;  // 存储完整哈希值
    uint8_t probe_distance;  // 探测距离
} RobinHoodEntry;

// 技巧4: SIMD加速 (概念性代码)
// 使用SSE/AVX指令同时比较多个槽位
#if defined(__SSE2__)
#include <emmintrin.h>

// SIMD查找4个连续槽位
static inline int simd_find_slot(const int* keys, __m128i target) {
    __m128i k = _mm_loadu_si128((__m128i const*)keys);
    __m128i cmp = _mm_cmpeq_epi32(k, target);
    int mask = _mm_movemask_ps(_mm_castsi128_ps(cmp));
    return mask;  // 返回匹配的位掩码
}
#endif

// 技巧5: 内存对齐优化
// 确保哈希表数组按缓存行对齐 (通常64字节)
#define CACHE_LINE_SIZE 64

void* aligned_malloc(size_t size) {
    void* ptr = nullptr;
    #ifdef _WIN32
    ptr = _aligned_malloc(size, CACHE_LINE_SIZE);
    #else
    posix_memalign(&ptr, CACHE_LINE_SIZE, size);
    #endif
    return ptr;
}
