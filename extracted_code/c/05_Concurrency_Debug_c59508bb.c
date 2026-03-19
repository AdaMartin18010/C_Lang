/*
 * Auto-generated from: 06_Thinking_Representation\01_Decision_Trees\05_Concurrency_Debug.md
 * Line: 143
 * Language: c
 * Block ID: c59508bb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 包装锁操作以追踪
void tracked_lock(pthread_mutex_t* m, const char* file, int line) {
    printf("[%s:%d] Locking at %s:%d\n",
           __FILE__, __LINE__, file, line);
    pthread_mutex_lock(m);
}

#define LOCK(m) tracked_lock(m, __FILE__, __LINE__)
