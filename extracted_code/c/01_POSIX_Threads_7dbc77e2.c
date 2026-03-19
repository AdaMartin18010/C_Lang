/*
 * Auto-generated from: 03_System_Technology_Domains\14_Concurrency_Parallelism\01_POSIX_Threads.md
 * Line: 1990
 * Language: c
 * Block ID: 7dbc77e2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 陷阱10：使用非线程安全函数 */
void *unsafe_worker(void *arg) {
    int id = *(int*)arg;

    // strerror - 返回静态缓冲区，非线程安全
    char *msg = strerror(EINVAL);
    printf("Thread %d: %s\n", id, msg);

    // strtok - 使用静态指针保存状态
    char str[] = "a,b,c";
    char *token = strtok(str, ",");
    while (token) {
        printf("%s\n", token);
        token = strtok(NULL, ",");
    }

    // rand - 使用全局状态
    int r = rand();

    // asctime, ctime - 使用静态缓冲区
    time_t now = time(NULL);
    char *time_str = ctime(&now);

    return NULL;
}

/* 修复方案：使用线程安全版本 */
void *safe_worker(void *arg) {
    int id = *(int*)arg;

    // 使用线程安全版本
    char buf[256];
    strerror_r(EINVAL, buf, sizeof(buf));  // POSIX线程安全版本
    printf("Thread %d: %s\n", id, buf);

    // strtok_r 使用显式状态
    char str[] = "a,b,c";
    char *saveptr;
    char *token = strtok_r(str, ",", &saveptr);
    while (token) {
        printf("%s\n", token);
        token = strtok_r(NULL, ",", &saveptr);
    }

    // 使用线程局部随机状态
    unsigned int seed = id;  // 每个线程不同种子
    int r = rand_r(&seed);

    // ctime_r 使用用户提供的缓冲区
    time_t now = time(NULL);
    char time_buf[26];
    ctime_r(&now, time_buf);

    return NULL;
}

/* 线程安全函数对照表 */
// 不安全        → 线程安全
// strerror      → strerror_r
// strtok        → strtok_r
// rand/srand    → rand_r
// ctime         → ctime_r
// asctime       → asctime_r
// gethostbyname → gethostbyname_r
// localtime     → localtime_r
// gmtime        → gmtime_r
