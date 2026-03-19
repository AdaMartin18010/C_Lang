/*
 * Auto-generated from: 00_VERSION_TRACKING\LLVM_22_C23_Update.md
 * Line: 104
 * Language: c
 * Block ID: d04331a4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 传统方式：使用 goto (混乱且容易出错)
void process_matrix_traditional(int rows, int cols, int matrix[rows][cols]) {
    int found = 0;
    int target_row = -1, target_col = -1;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (matrix[i][j] == TARGET_VALUE) {
                target_row = i;
                target_col = j;
                found = 1;
                goto done;  // 需要 goto 跳出多层循环
            }
        }
    }
done:
    if (found) {
        printf("Found at (%d, %d)\n", target_row, target_col);
    }
}
