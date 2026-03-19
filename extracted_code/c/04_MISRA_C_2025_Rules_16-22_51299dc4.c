/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\04_MISRA_C_2025_Rules_16-22.md
 * Line: 326
 * Language: c
 * Block ID: 51299dc4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 不符合规范 */
pthread_t thread;
pthread_create(&thread, NULL, thread_func, NULL);
/* 未检查返回值 */

/* 符合规范 */
pthread_t thread;
int ret = pthread_create(&thread, NULL, thread_func, NULL);
if (ret != 0) {
    /* 处理线程创建失败 */
}
