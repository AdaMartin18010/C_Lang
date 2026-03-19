/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\01_Sorting_Algorithms_Complete.md
 * Line: 2594
 * Language: c
 * Block ID: 6b57b1bf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 多字段排序 - 稳定排序保证次要字段顺序
 */
typedef struct {
    int age;
    char name[32];
    double score;
} Student;

// 先按分数排序，分数相同按年龄排序
void sort_students(Student students[], size_t n) {
    // 使用稳定排序，先按次要字段（年龄）排序
    // 再按主要字段（分数）排序
}
