/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\03_Hash_Table_Implementation.md
 * Line: 352
 * Language: c
 * Block ID: a86ce384
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 二次探测 (Quadratic Probing)
 * h(k, i) = (h'(k) + c1*i + c2*i^2) % m
 *
 * 常用: c1=0, c2=1
 * h(k, i) = (h'(k) + i^2) % m
 *
 * 探查序列: 0, 1, 4, 9, 16, 25...
 */
unsigned int hash_quadratic_probe(unsigned int h, size_t i, size_t table_size) {
    // c1 = 0.5, c2 = 0.5 时序列为: 0, 1, 3, 6, 10, 15... (三角形数)
    return (h + i + i * i / 2) % table_size;
}
