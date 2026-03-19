/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\04_Cache_Lab.md
 * Line: 66
 * Language: c
 * Block ID: f72ce47c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 缓存命中 (Hit)
// 数据在缓存中，直接返回

// 缓存未命中 (Miss)
// - Cold Miss: 首次访问，缓存中不存在
// - Conflict Miss: 不同数据映射到同一缓存行
// - Capacity Miss: 缓存容量不足

// 驱逐 (Eviction)
// 缓存满时需要替换旧行（LRU策略）
