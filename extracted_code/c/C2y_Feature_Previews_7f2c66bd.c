/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_Feature_Previews.md
 * Line: 154
 * Language: c
 * Block ID: 7f2c66bd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============ C89-C23 方案: 使用goto ============
// 问题: 被普遍认为降低可读性，容易出错

int found = 0;
for (int i = 0; i < N && !found; i++) {
    for (int j = 0; j < M && !found; j++) {
        if (matrix[i][j] == target) {
            found = 1;
            result_i = i;
            result_j = j;
        }
    }
}
// 或者使用goto
for (int i = 0; i < N; i++) {
    for (int j = 0; j < M; j++) {
        if (matrix[i][j] == target) {
            goto found;
        }
    }
}
found:
// 处理找到的情况

// ============ C2y 方案: Named Loops ============
// 更清晰、结构化、无goto

search: for (int i = 0; i < N; i++) {
    for (int j = 0; j < M; j++) {
        if (matrix[i][j] == target) {
            result_i = i;
            result_j = j;
            break search;
        }
    }
}
