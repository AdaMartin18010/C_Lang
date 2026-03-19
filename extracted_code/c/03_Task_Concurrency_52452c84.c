/*
 * Auto-generated from: 17_Ada_SPARK\01_Ada_Basics\03_Task_Concurrency.md
 * Line: 309
 * Language: c
 * Block ID: 52452c84
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
int buffer[10], count = 0;

void* producer(void* arg) {
    pthread_mutex_lock(&mutex);
    while (count == 10)
        pthread_cond_wait(&not_full, &mutex);
    buffer[count++] = *(int*)arg;
    pthread_cond_signal(&not_empty);
    pthread_mutex_unlock(&mutex);
}
