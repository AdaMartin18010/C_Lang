/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\04_Cache_Lab.md
 * Line: 81
 * Language: c
 * Block ID: 2489ae5f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 时间局部性：近期访问的数据很可能再次被访问
for (int i = 0; i < n; i++) {
    sum += a[0];  // 重复访问同一元素
}

// 空间局部性：访问某地址后，附近地址很可能被访问
for (int i = 0; i < n; i++) {
    sum += a[i];  // 顺序访问相邻元素
}
