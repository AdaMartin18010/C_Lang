/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_New_Proposals_2026.md
 * Line: 156
 * Language: c
 * Block ID: 6e011ad4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 矩阵搜索，找到后立即退出所有循环
search: for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
        if (matrix[i][j] == target) {
            result_row = i;
            result_col = j;
            break search;  // 直接跳出外层search循环
        }
    }
}

// 嵌套continue
outer: while (condition1) {
    inner: while (condition2) {
        if (skip_outer) {
            continue outer;  // 直接继续外层循环
        }
        // ...
    }
}
