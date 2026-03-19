/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\01_SIMD_Vectorization.md
 * Line: 2327
 * Language: c
 * Block ID: 60732963
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 不处理尾部元素
for (int i = 0; i < n - 7; i += 8) {  // 如果n不是8的倍数，漏掉末尾
    // SIMD处理
}

// ✅ 正确: 完整处理
int i = 0;
for (; i <= n - 8; i += 8) {
    // SIMD处理8个元素
}
for (; i < n; i++) {  // 处理剩余1-7个元素
    // 标量处理
}
