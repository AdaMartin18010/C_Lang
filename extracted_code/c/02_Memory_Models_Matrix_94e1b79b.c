/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\02_Memory_Models_Matrix.md
 * Line: 137
 * Language: c
 * Block ID: 94e1b79b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 方式
_Atomic int counter = 0;
void increment(void) {
    atomic_fetch_add(&counter, 1, memory_order_relaxed);
}

// POSIX 方式
int counter = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void increment(void) {
    pthread_mutex_lock(&mutex);
    counter++;
    pthread_mutex_unlock(&mutex);
}

// Linux内核方式
atomic_t counter = ATOMIC_INIT(0);
void increment(void) {
    atomic_inc(&counter);
}
