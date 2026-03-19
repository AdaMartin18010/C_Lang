/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\02_Searching_Algorithms.md
 * Line: 1442
 * Language: c
 * Block ID: 1373e2a9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 数据库索引二分搜索示例
 */
typedef struct {
    int id;
    char name[32];
    double salary;
} Employee;

typedef struct {
    int id;
    size_t record_offset;
} IndexEntry;

// 在索引中搜索
Employee* search_by_id(IndexEntry index[], size_t n,
                       int id, FILE *data_file) {
    // 二分搜索索引
    int idx = binary_search_basic(index->id, n, id);
    if (idx < 0) return NULL;

    // 读取实际记录
    Employee *emp = malloc(sizeof(Employee));
    fseek(data_file, index[idx].record_offset, SEEK_SET);
    fread(emp, sizeof(Employee), 1, data_file);
    return emp;
}
