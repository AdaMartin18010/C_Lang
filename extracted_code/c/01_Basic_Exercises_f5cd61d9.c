/*
 * Auto-generated from: 12_Practice_Exercises\01_Basic_Exercises.md
 * Line: 2022
 * Language: c
 * Block ID: f5cd61d9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void create_matrix(int n) {
    int **matrix = malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        matrix[i] = malloc(n * sizeof(int));
    }
    // 填充矩阵...

    // 先释放每行
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    // 再释放指针数组
    free(matrix);
}
