/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\09_MISRA_C_2023_Rules_9.md
 * Line: 296
 * Language: c
 * Block ID: fbc4dd91
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 无保护的共享数据 */
int shared_counter = 0;

void increment_unsafe(void)
{
    shared_counter++;  /* 数据竞争！ */
}

/* ❌ 违反 - 检查-使用竞争 */
if (shared_ptr != NULL) {
    /* 另一个线程可能在这里释放 */
    use(shared_ptr);  /* 使用已释放内存 */
}
