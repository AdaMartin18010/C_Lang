/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\03_Hash_Table_Implementation.md
 * Line: 337
 * Language: c
 * Block ID: 1a47d24d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 线性探测 (Linear Probing)
 * h(k, i) = (h'(k) + i) % m
 *
 * 优点: 简单，缓存友好
 * 缺点: 容易产生聚集 (clustering)
 */
unsigned int hash_linear_probe(unsigned int h, size_t i, size_t table_size) {
    return (h + i) % table_size;
}
