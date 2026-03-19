/*
 * Auto-generated from: 03_System_Technology_Domains\14_Concurrency_Parallelism\01_POSIX_Threads.md
 * Line: 2221
 * Language: c
 * Block ID: 4a83f69f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 陷阱13：资源泄漏在取消时 */
void *worker_cleanup_bug(void *arg) {
    FILE *fp = fopen("temp.txt", "w");
    if (!fp) return NULL;

    // 分配内存
    char *buffer = malloc(1024);

    // 长时间操作（取消点）
    for (int i = 0; i < 100; i++) {
        fwrite(buffer, 1, 1024, fp);  // 可能是取消点
    }

    // 如果在这里被取消，buffer和fp泄漏！
    free(buffer);
    fclose(fp);
    return NULL;
}

/* 修复方案：使用pthread_cleanup_push/pop */
void cleanup_file(void *arg) {
    FILE *fp = arg;
    if (fp) {
        printf("Cleanup: closing file\n");
        fclose(fp);
    }
}

void cleanup_memory(void *arg) {
    void *ptr = arg;
    if (ptr) {
        printf("Cleanup: freeing memory\n");
        free(ptr);
    }
}

void *worker_cleanup_fixed(void *arg) {
    FILE *fp = fopen("temp.txt", "w");
    if (!fp) return NULL;

    pthread_cleanup_push(cleanup_file, fp);

    char *buffer = malloc(1024);
    pthread_cleanup_push(cleanup_memory, buffer);

    // 长时间操作
    for (int i = 0; i < 100; i++) {
        fwrite(buffer, 1, 1024, fp);
    }

    // 正常退出时也执行清理
    pthread_cleanup_pop(1);  // 1 = 执行清理
    pthread_cleanup_pop(1);
    return NULL;
}
