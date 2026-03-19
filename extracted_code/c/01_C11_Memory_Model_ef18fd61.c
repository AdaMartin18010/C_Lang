/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\01_C11_Memory_Model.md
 * Line: 84
 * Language: c
 * Block ID: ef18fd61
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C11标准定义的内存序 */
typedef enum {
    memory_order_relaxed = 0,  /* 无同步保证，仅原子性 */
    memory_order_consume = 1,  /* 消费序 (少用) */
    memory_order_acquire = 2,  /* 获取序 */
    memory_order_release = 3,  /* 释放序 */
    memory_order_acq_rel = 4,  /* 获取-释放 */
    memory_order_seq_cst = 5   /* 顺序一致性 (默认) */
} memory_order;
