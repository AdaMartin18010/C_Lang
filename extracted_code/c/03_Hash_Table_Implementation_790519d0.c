/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\03_Hash_Table_Implementation.md
 * Line: 370
 * Language: c
 * Block ID: 790519d0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 双重哈希 (Double Hashing)
 * h(k, i) = (h1(k) + i * h2(k)) % m
 *
 * h2(k) 必须: 1. 永不为0  2. 与m互质
 */
unsigned int hash1(int key, size_t table_size) {
    return (unsigned int)(key % (int)table_size);
}

unsigned int hash2(int key, size_t table_size) {
    // 确保结果不为0且与table_size互质
    return 1 + (unsigned int)(key % ((int)table_size - 1));
}

unsigned int hash_double(unsigned int h1, unsigned int h2, size_t i, size_t table_size) {
    return (h1 + i * h2) % table_size;
}
