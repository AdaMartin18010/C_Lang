/*
 * Auto-generated from: 06_Thinking_Representation\01_Decision_Trees\05_Concurrency_Debug.md
 * Line: 122
 * Language: c
 * Block ID: 4ebc623b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define DEBUG_LOG(fmt, ...) \
    printf("[%s:%d][%lu] " fmt "\n", \
           __FILE__, __LINE__, pthread_self(), ##__VA_ARGS__)

void thread_func(void* arg) {
    DEBUG_LOG("Starting, arg=%p", arg);

    DEBUG_LOG("Locking mutex");
    pthread_mutex_lock(&mutex);

    DEBUG_LOG("In critical section");
    // ...

    DEBUG_LOG("Unlocking mutex");
    pthread_mutex_unlock(&mutex);
}
