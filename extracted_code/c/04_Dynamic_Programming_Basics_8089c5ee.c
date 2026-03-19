/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\04_Dynamic_Programming_Basics.md
 * Line: 1355
 * Language: c
 * Block ID: 8089c5ee
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 三角形最小路径和 - 原地修改
 * @param triangle 三角形数组
 * @param triangleSize 行数
 */
int minimum_total(int **triangle, int triangleSize) {
    // 从倒数第二行向上更新
    for (int i = triangleSize - 2; i >= 0; i--) {
        for (int j = 0; j <= i; j++) {
            // 选择下面两个中的较小者
            int min_below = triangle[i + 1][j] < triangle[i + 1][j + 1] ?
                            triangle[i + 1][j] : triangle[i + 1][j + 1];
            triangle[i][j] += min_below;
        }
    }
    return triangle[0][0];
}
