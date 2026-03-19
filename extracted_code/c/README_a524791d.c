/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\README.md
 * Line: 141
 * Language: c
 * Block ID: a524791d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 利用局部性的优化示例

// 差：按列访问（跳跃式）
for (int j = 0; j < n; j++) {
    for (int i = 0; i < n; i++) {
        sum += matrix[i][j];  // 缓存未命中率高
    }
}

// 好：按行访问（连续）
for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
        sum += matrix[i][j];  // 充分利用缓存行
    }
}
