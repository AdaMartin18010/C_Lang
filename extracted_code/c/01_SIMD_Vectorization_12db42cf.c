/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\01_SIMD_Vectorization.md
 * Line: 2178
 * Language: c
 * Block ID: 12db42cf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 循环携带依赖
for (int i = 1; i < n; i++) {
    a[i] = a[i-1] + b[i];  // RAW依赖，不可向量化
}

// ✅ 解决方案1: 算法重写
float prev = a[0];
for (int i = 1; i < n; i++) {
    float curr = prev + b[i];
    a[i] = curr;
    prev = curr;
}

// ✅ 解决方案2: 使用前缀和算法
// 采用并行扫描算法 (Blelloch scan)
