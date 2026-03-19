/*
 * Auto-generated from: 03_System_Technology_Domains\14_Concurrency_Parallelism\01_POSIX_Threads.md
 * Line: 2061
 * Language: c
 * Block ID: 5e0108f6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 陷阱11：返回局部变量地址 */
void *worker_broken(void *arg) {
    int local_result = 42;  // 栈局部变量
    pthread_exit(&local_result);  // BUG: 返回栈地址！
}

void *worker_return(void *arg) {
    int local_result = 42;
    return &local_result;  // 同样的问题
}

int demo_stack_bug() {
    pthread_t thread;
    pthread_create(&thread, NULL, worker_broken, NULL);

    void *result;
    pthread_join(thread, &result);

    // result指向已销毁的栈帧，未定义行为
    printf("Result: %d\n", *(int*)result);  // 可能崩溃或输出垃圾

    return 0;
}

/* 修复方案1：使用堆分配 */
void *worker_heap(void *arg) {
    int *result = malloc(sizeof(int));
    *result = 42;
    pthread_exit(result);
}

/* 修复方案2：使用传入的缓冲区 */
typedef struct {
    int input;
    int output;
} thread_io_t;

void *worker_io(void *arg) {
    thread_io_t *io = arg;
    io->output = io->input * 2;
    return NULL;
}

int demo_fixed() {
    pthread_t thread;
    thread_io_t io = {.input = 21};
    pthread_create(&thread, NULL, worker_io, &io);
    pthread_join(thread, NULL);
    printf("Result: %d\n", io.output);  // 安全：主线程栈仍然有效
    return 0;
}

/* 修复方案3：使用线程参数直接传递值 */
void *worker_value(void *arg) {
    intptr_t input = (intptr_t)arg;
    intptr_t result = input * 2;
    pthread_exit((void*)result);  // 值直接编码在指针中
}

int demo_value() {
    pthread_t thread;
    pthread_create(&thread, NULL, worker_value, (void*)21);

    void *result;
    pthread_join(thread, &result);
    printf("Result: %ld\n", (intptr_t)result);
    return 0;
}
