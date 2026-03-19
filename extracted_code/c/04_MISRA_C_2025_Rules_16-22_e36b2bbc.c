/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\04_MISRA_C_2025_Rules_16-22.md
 * Line: 306
 * Language: c
 * Block ID: e36b2bbc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 不符合规范 - 临界区过大 */
pthread_mutex_lock(&mutex);
/* 长时间操作 */
process_large_dataset();
/* 更多操作 */
pthread_mutex_unlock(&mutex);

/* 符合规范 - 最小化临界区 */
pthread_mutex_lock(&mutex);
SharedData data = shared_resource;  /* 快速复制 */
pthread_mutex_unlock(&mutex);

process_large_dataset(data);  /* 在锁外处理 */
