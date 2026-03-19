/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_TLA_Plus\05_C_Verification_with_TLA.md
 * Line: 60
 * Language: c
 * Block ID: 4395f789
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ========== C语言版本 ========== */
/* 简单的计数器 - C实现 */
#include <pthread.h>

int counter = 0;
pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

void* increment_thread(void* arg) {
    for (int i = 0; i < 100; i++) {
        pthread_mutex_lock(&counter_mutex);
        counter++;
        pthread_mutex_unlock(&counter_mutex);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, increment_thread, NULL);
    pthread_create(&t2, NULL, increment_thread, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("counter = %d\n", counter);
    return 0;
}
