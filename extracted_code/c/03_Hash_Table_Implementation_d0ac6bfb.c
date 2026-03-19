/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\03_Hash_Table_Implementation.md
 * Line: 302
 * Language: c
 * Block ID: d0ac6bfb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 组合多个字段的哈希值
 * 用于结构体哈希
 */
unsigned int combine_hash(unsigned int h1, unsigned int h2) {
    // Boost hash_combine 算法
    return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
}

// 结构体哈希示例
typedef struct {
    int id;
    char name[64];
    double score;
} Student;

unsigned int student_hash(const Student* s, size_t table_size) {
    unsigned int h = fnv1a_hash(s->name);
    h = combine_hash(h, (unsigned int)s->id);
    h = combine_hash(h, (unsigned int)(s->score * 1000));
    return h % table_size;
}
