/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\04_MISRA_C_2025_Rules_16-22.md
 * Line: 344
 * Language: c
 * Block ID: 579ab3aa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 不符合规范 - 忙等待 */
while (!condition_met) {
    /* 浪费CPU */
}

/* 符合规范 - 条件变量 */
pthread_mutex_lock(&mutex);
while (!condition_met) {
    pthread_cond_wait(&cond, &mutex);
}
/* 处理条件 */
pthread_mutex_unlock(&mutex);
