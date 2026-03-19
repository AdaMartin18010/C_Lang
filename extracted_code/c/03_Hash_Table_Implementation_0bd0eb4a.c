/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\03_Hash_Table_Implementation.md
 * Line: 189
 * Language: c
 * Block ID: 0bd0eb4a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 乘法哈希法
 * h(k) = floor(m * (k * A mod 1))
 * 其中 A ≈ (√5 - 1) / 2 ≈ 0.6180339887 (黄金比例共轭)
 *
 * 优点: m 可以是2的幂次，计算效率高
 */
#define A 0.6180339887

unsigned int hash_multiplication(int key, size_t table_size) {
    double fractional = key * A - (int)(key * A);
    return (unsigned int)(table_size * fractional);
}
