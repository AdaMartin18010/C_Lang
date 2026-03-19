/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\04_Concurrency_Patterns.md
 * Line: 380
 * Language: c
 * Block ID: 6c3251d2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
int data = 0;

void *reader(void *arg) {
    pthread_rwlock_rdlock(&rwlock);  // 多个读者可以同时持有
    printf("Read: %d\n", data);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

void *writer(void *arg) {
    pthread_rwlock_wrlock(&rwlock);  // 独占访问
    data++;
    printf("Write: %d\n", data);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}
