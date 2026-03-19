/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\03_Algorithm_Lower_Bounds.md
 * Line: 649
 * Language: c
 * Block ID: 12ff0f7a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 优先队列操作下界分析

// 基于数组的简单实现
// INSERT: O(n) - 需要保持有序
// EXTRACT-MIN: O(1)

// 基于堆的实现
// INSERT: O(log n) - 需要上浮
// EXTRACT-MIN: O(log n) - 需要下沉

// 下界证明（基于决策树）：
// 插入n个元素后提取k个最小值
// 需要至少 n*log(k) 次比较
// 堆实现达到此下界
