/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\01_Sorting_Algorithms_Complete.md
 * Line: 2569
 * Language: c
 * Block ID: 075ace9e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 外部排序 - 处理无法放入内存的数据
 */
#include <stdio.h>
#include <stdlib.h>

#define CHUNK_SIZE 1000000  // 每次处理100万个整数

typedef struct {
    FILE *file;
    int current_value;
    bool has_value;
} MergeSource;

// 外部归并排序
void external_sort(const char *input_file, const char *output_file) {
    // 1. 分块读取，每块内存排序后写入临时文件
    // 2. 使用k路归并合并所有临时文件
    // 3. 输出到目标文件
}
