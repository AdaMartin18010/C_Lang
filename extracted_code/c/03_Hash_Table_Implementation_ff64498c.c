/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\03_Hash_Table_Implementation.md
 * Line: 1094
 * Language: c
 * Block ID: ff64498c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 布隆过滤器 (Bloom Filter)
 * 空间效率极高的概率型数据结构
 * 可能存在假阳性，但不会有假阴性
 */

typedef struct {
    unsigned char* bits;
    size_t size;          // 位数组大小
    size_t hash_count;    // 哈希函数数量
} BloomFilter;

// 初始化
BloomFilter* bloom_create(size_t expected_items, double false_positive_rate) {
    // 计算最优参数
    // m = -n * ln(p) / (ln(2)^2)
    // k = m/n * ln(2)

    double ln2 = 0.69314718056;
    size_t m = (size_t)(-expected_items * log(false_positive_rate) / (ln2 * ln2));
    size_t k = (size_t)(m / expected_items * ln2);

    BloomFilter* bf = (BloomFilter*)malloc(sizeof(BloomFilter));
    bf->size = m;
    bf->hash_count = k;
    bf->bits = (unsigned char*)calloc((m + 7) / 8, 1);

    return bf;
}

// 设置位
static void set_bit(BloomFilter* bf, size_t idx) {
    bf->bits[idx / 8] |= (1 << (idx % 8));
}

// 检查位
static bool test_bit(const BloomFilter* bf, size_t idx) {
    return (bf->bits[idx / 8] & (1 << (idx % 8))) != 0;
}

// 添加元素
void bloom_add(BloomFilter* bf, const char* item) {
    for (size_t i = 0; i < bf->hash_count; i++) {
        unsigned int h = murmur3_32(item, strlen(item), (unsigned int)i);
        set_bit(bf, h % bf->size);
    }
}

// 检查可能存在
bool bloom_might_contain(const BloomFilter* bf, const char* item) {
    for (size_t i = 0; i < bf->hash_count; i++) {
        unsigned int h = murmur3_32(item, strlen(item), (unsigned int)i);
        if (!test_bit(bf, h % bf->size)) {
            return false;  // 肯定不存在
        }
    }
    return true;  // 可能存在 (假阳性)
}
