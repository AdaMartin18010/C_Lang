/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 528
 * Language: c
 * Block ID: e0a1a3d5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 示例2：多维数组的_Countof使用
 * 演示如何获取各维度大小
 */
#include <stdio.h>

int main(void) {
    int matrix[3][4];
    int cube[2][3][4];

    printf("2D Array matrix[3][4]:\n");
    printf("  _Countof(matrix):      %zu (rows)\n", _Countof(matrix));
    printf("  _Countof(matrix[0]):   %zu (cols)\n", _Countof(matrix[0]));
    printf("  Total elements:        %zu\n",
           _Countof(matrix) * _Countof(matrix[0]));

    printf("\n3D Array cube[2][3][4]:\n");
    printf("  _Countof(cube):        %zu (dim1)\n", _Countof(cube));
    printf("  _Countof(cube[0]):     %zu (dim2)\n", _Countof(cube[0]));
    printf("  _Countof(cube[0][0]):  %zu (dim3)\n", _Countof(cube[0][0]));
    printf("  Total elements:        %zu\n",
           _Countof(cube) * _Countof(cube[0]) * _Countof(cube[0][0]));

    // 遍历二维数组
    printf("\nTraversing matrix:\n");
    for (size_t i = 0; i < _Countof(matrix); i++) {
        for (size_t j = 0; j < _Countof(matrix[i]); j++) {
            matrix[i][j] = i * _Countof(matrix[i]) + j;
            printf("  matrix[%zu][%zu] = %d\n", i, j, matrix[i][j]);
        }
    }

    return 0;
}
