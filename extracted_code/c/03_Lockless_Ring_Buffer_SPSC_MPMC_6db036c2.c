/*
 * Auto-generated from: 03_System_Technology_Domains\09_High_Performance_Log\03_Lockless_Ring_Buffer_SPSC_MPMC.md
 * Line: 344
 * Language: c
 * Block ID: 6db036c2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用32位索引在高负载下可能ABA
// 解决方案：使用64位索引或Tagged Pointer
// 64位系统使用高16位作为tag
#define TAG_BITS 16
#define INDEX_BITS 48
#define MAKE_INDEX(seq, tag) (((uint64_t)(tag) << INDEX_BITS) | (seq))
