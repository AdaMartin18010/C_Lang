/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\04_MISRA_C_2025_Rules_16-22.md
 * Line: 371
 * Language: c
 * Block ID: e4999b9c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 符合规范 */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
/* 使用... */
pthread_mutex_destroy(&mutex);  /* 清理 */
