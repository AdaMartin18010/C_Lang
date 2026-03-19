/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\04_Concurrency_Patterns.md
 * Line: 110
 * Language: c
 * Block ID: cc8be5c4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 方式1: 正常返回
void *thread_func(void *arg) {
    // ... 工作
    return result;  // 返回值通过pthread_join获取
}

// 方式2: pthread_exit
void *thread_func(void *arg) {
    // ... 工作
    pthread_exit(result);  // 等效于return
}

// 方式3: 取消
pthread_cancel(tid);  // 请求取消线程

// 清理处理程序
void cleanup(void *arg) {
    free(arg);  // 确保资源释放
}

pthread_cleanup_push(cleanup, ptr);
// ... 可能取消的操作
pthread_cleanup_pop(1);
