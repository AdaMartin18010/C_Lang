/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\04_MISRA_C_2025_Rules_16-22.md
 * Line: 291
 * Language: c
 * Block ID: fd46b2a8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 不符合规范 */
_Atomic int counter;
int local = counter;  /* 非原子读取 */

/* 符合规范 */
int local = atomic_load(&counter);
atomic_store(&counter, 42);
int new_val = atomic_fetch_add(&counter, 1);
