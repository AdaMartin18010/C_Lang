/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\02_Lock_Free_Queues.md
 * Line: 43
 * Language: c
 * Block ID: d6e686a7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// memory_order_relaxed: 最宽松的排序，只保证原子性
// memory_order_consume: 数据依赖顺序（极少使用）
// memory_order_acquire: 获取语义，之后的读写不能重排到前面
// memory_order_release: 释放语义，之前的读写不能重排到后面
// memory_order_acq_rel: 获取+释放（用于读-修改-写操作）
// memory_order_seq_cst: 顺序一致性（默认，最强）

// 典型使用模式
// Load(Load-Consume/Acquire): 读操作
// Store(Store-Release): 写操作
// Read-Modify-Write(Acq_Rel/Seq_Cst): CAS/Exchange等
