/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\04_MISRA_C_2025_Rules_16-22.md
 * Line: 223
 * Language: c
 * Block ID: f5b3d6bd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 不符合规范 - 数据竞争 */
int shared_var = 0;

void* thread_func(void* arg) {
    shared_var++;  /* 非原子操作，数据竞争 */
    return NULL;
}

/* 符合规范 - 使用原子操作 */
_Atomic int shared_var = 0;

void* thread_func(void* arg) {
    shared_var++;  /* 原子操作 */
    return NULL;
}
