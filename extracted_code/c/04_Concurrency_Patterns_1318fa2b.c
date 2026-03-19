/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\04_Concurrency_Patterns.md
 * Line: 168
 * Language: c
 * Block ID: 1318fa2b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 方式1: GCC扩展
__thread int thread_local_var = 0;

// 方式2: POSIX标准
pthread_key_t key;

void init_tls() {
    pthread_key_create(&key, free);  // 可选的析构函数
}

void set_value(void *value) {
    pthread_setspecific(key, value);
}

void *get_value() {
    return pthread_getspecific(key);
}
