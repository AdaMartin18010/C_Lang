/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\03_Hash_Table_Implementation.md
 * Line: 207
 * Language: c
 * Block ID: 7ca1573e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * FNV-1a (Fowler-Noll-Vo) 哈希算法
 * 优秀的字符串哈希函数，分布均匀，速度快
 */
unsigned int fnv1a_hash(const char* str) {
    const unsigned int FNV_PRIME = 16777619u;
    const unsigned int FNV_OFFSET = 2166136261u;

    unsigned int hash = FNV_OFFSET;
    while (*str) {
        hash ^= (unsigned char)(*str++);
        hash *= FNV_PRIME;
    }
    return hash;
}
