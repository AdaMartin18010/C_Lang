/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\01_Standards_Comparison_Matrix.md
 * Line: 949
 * Language: c
 * Block ID: 14084c38
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 真正的多维VLA */
void matrix_multiply(int n, int m, int p,
                     double a[n][m],
                     double b[m][p],
                     double result[n][p]) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p; j++) {
            result[i][j] = 0;
            for (int k = 0; k < m; k++) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}
