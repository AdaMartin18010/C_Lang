/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\04_MISRA_C_2025_Rules_16-22.md
 * Line: 245
 * Language: c
 * Block ID: 92e5f5f0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 不符合规范 - 缺少解锁 */
pthread_mutex_lock(&mutex);
/* 条件分支中可能提前返回 */
if (error) {
    return;  /* 违反规则 - 未解锁 */
}
pthread_mutex_unlock(&mutex);

/* 符合规范 - 确保解锁 */
pthread_mutex_lock(&mutex);
/* 临界区代码 */
pthread_mutex_unlock(&mutex);  /* 总是执行解锁 */
