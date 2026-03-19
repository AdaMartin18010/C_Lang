/*
 * Auto-generated from: 00_VERSION_TRACKING\LLVM_22_C23_Update.md
 * Line: 127
 * Language: c
 * Block ID: f0818f0f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C2y Named Loops：清晰、安全
#include <stdio.h>

void process_matrix_named(int rows, int cols, int matrix[rows][cols]) {
    int found = 0;
    int target_row = -1, target_col = -1;

    outer: for (int i = 0; i < rows; i++) {
        inner: for (int j = 0; j < cols; j++) {
            if (matrix[i][j] == TARGET_VALUE) {
                target_row = i;
                target_col = j;
                found = 1;
                break outer;  // 直接跳出外层循环
            }
        }
    }

    if (found) {
        printf("Found at (%d, %d)\n", target_row, target_col);
    }
}
