/*
 * Auto-generated from: 17_Ada_SPARK\08_Comparison_Matrices\01_Ada_C_Rust_Safety_Matrix.md
 * Line: 153
 * Language: c
 * Block ID: 286ef31b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C - pthread，无安全保证
pthread_mutex_t lock;
int counter = 0;

void* thread_fn(void* arg) {
    pthread_mutex_lock(&lock);
    counter++;  // 忘记解锁会怎样？
    pthread_mutex_unlock(&lock);
}
