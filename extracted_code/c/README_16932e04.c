/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\README.md
 * Line: 224
 * Language: c
 * Block ID: 16932e04
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// POSIX 风格 - 创建线程
#include <pthread.h>

void* thread_function(void* arg) {
    int* value = (int*)arg;
    printf("Thread value: %d\n", *value);
    return NULL;
}

int main() {
    pthread_t thread;
    int value = 42;
    pthread_create(&thread, NULL, thread_function, &value);
    pthread_join(thread, NULL);
    return 0;
}
