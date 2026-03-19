/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\03_Hash_Table_Implementation.md
 * Line: 175
 * Language: c
 * Block ID: 77d2fb84
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 除法哈希法
 * h(k) = k % m
 *
 * 建议: m 选择质数，避免与键的分布模式相关
 */
unsigned int hash_division(int key, size_t table_size) {
    return (unsigned int)(key % (int)table_size);
}
