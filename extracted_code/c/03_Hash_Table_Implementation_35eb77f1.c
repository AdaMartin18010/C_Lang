/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\03_Hash_Table_Implementation.md
 * Line: 784
 * Language: c
 * Block ID: 35eb77f1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 负载因子阈值
#define MAX_LOAD_FACTOR 0.75   // 超过则扩容
#define MIN_LOAD_FACTOR 0.10   // 低于则缩容 (可选)

// 扩容触发条件
bool should_expand(const HashTable* table) {
    return (double)table->size / table->capacity > MAX_LOAD_FACTOR;
}

bool should_shrink(const HashTable* table) {
    return table->capacity > INITIAL_CAPACITY &&
           (double)table->size / table->capacity < MIN_LOAD_FACTOR;
}
