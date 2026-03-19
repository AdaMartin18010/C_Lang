/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\01_C11_Memory_Model.md
 * Line: 306
 * Language: c
 * Block ID: a2357bc6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

_Atomic(int) counter = 0;

/* 加减 */
int old = atomic_fetch_add(&counter, 5);   /* counter += 5, 返回旧值 */
int old2 = atomic_fetch_sub(&counter, 3);  /* counter -= 3 */

/* 位操作 */
atomic_fetch_or(&counter, 0x1);   /* 设置位 */
atomic_fetch_and(&counter, ~0x1); /* 清除位 */
atomic_fetch_xor(&counter, 0x1);  /* 翻转位 */

/* 自增自减 */
atomic_fetch_add(&counter, 1);  /* ++counter */
atomic_fetch_sub(&counter, 1);  /* --counter */

/* 显式内存序版本 */
atomic_fetch_add_explicit(&counter, 1, memory_order_relaxed);
