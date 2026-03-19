/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\03_Hash_Table_Implementation.md
 * Line: 244
 * Language: c
 * Block ID: 2a8cfda6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * MurmurHash3 32-bit 简化实现
 * 高质量的通用哈希函数
 */
unsigned int murmur3_32(const void* key, size_t len, unsigned int seed) {
    const unsigned int c1 = 0xcc9e2d51;
    const unsigned int c2 = 0x1b873593;
    const unsigned int r1 = 15;
    const unsigned int r2 = 13;
    const unsigned int m = 5;
    const unsigned int n = 0xe6546b64;

    const unsigned char* data = (const unsigned char*)key;
    unsigned int hash = seed;

    // 处理4字节块
    while (len >= 4) {
        unsigned int k = *(unsigned int*)data;

        k *= c1;
        k = (k << r1) | (k >> (32 - r1));
        k *= c2;

        hash ^= k;
        hash = (hash << r2) | (hash >> (32 - r2));
        hash = hash * m + n;

        data += 4;
        len -= 4;
    }

    // 处理剩余字节
    unsigned int k = 0;
    switch (len) {
        case 3: k ^= data[2] << 16;
        case 2: k ^= data[1] << 8;
        case 1: k ^= data[0];
                k *= c1;
                k = (k << r1) | (k >> (32 - r1));
                k *= c2;
                hash ^= k;
    }

    // 最终混合
    hash ^= len;
    hash ^= (hash >> 16);
    hash *= 0x85ebca6b;
    hash ^= (hash >> 13);
    hash *= 0xc2b2ae35;
    hash ^= (hash >> 16);

    return hash;
}
