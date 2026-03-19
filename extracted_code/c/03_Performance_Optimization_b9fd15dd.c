/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\03_Performance_Optimization.md
 * Line: 71
 * Language: c
 * Block ID: b9fd15dd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 缓存不友好
for (int j = 0; j < n; j++) {
    for (int i = 0; i < m; i++) {
        sum += matrix[i][j];  // 列优先，缓存缺失
    }
}

// ✅ 行优先访问
for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
        sum += matrix[i][j];
    }
}

// 循环展开
// 手动展开（编译器-O3自动做）
for (int i = 0; i < n; i += 4) {
    sum += arr[i];
    if (i+1 < n) sum += arr[i+1];
    if (i+2 < n) sum += arr[i+2];
    if (i+3 < n) sum += arr[i+3];
}
