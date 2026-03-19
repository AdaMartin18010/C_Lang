/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\06_Malloc_Lab.md
 * Line: 87
 * Language: c
 * Block ID: 3153fc56
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 首次适配 (First Fit)
// 从头到尾扫描，找到第一个足够大的空闲块
// 优点：简单，快速
// 缺点：可能产生碎片

// 下次适配 (Next Fit)
// 从上次查找位置继续扫描
// 优点：分散碎片
// 缺点：可能错过前面的合适块

// 最佳适配 (Best Fit)
// 扫描所有块，找到最小的足够大的块
// 优点：减少内部碎片
// 缺点：扫描开销大
