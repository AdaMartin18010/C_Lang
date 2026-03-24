/*
 * C23 线程安全时间函数示例
 * 编译: gcc -std=c23 -o 18_thread_safe_time 18_thread_safe_time.c -lpthread
 *       clang -std=c23 -o 18_thread_safe_time 18_thread_safe_time.c -lpthread
 */

#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

// 线程参数结构
typedef struct {
    int thread_id;
    int iterations;
} ThreadArg;

// 线程安全的时间转换
void* time_worker(void *arg) {
    ThreadArg *ta = (ThreadArg*)arg;
    
    for (int i = 0; i < ta->iterations; i++) {
        time_t now = time(NULL);
        struct tm local_tm;
        struct tm gm_tm;
        
        // 线程安全的本地时间转换
        if (localtime_r(&now, &local_tm) != NULL) {
            char time_str[64];
            strftime(time_str, sizeof(time_str), 
                     "%Y-%m-%d %H:%M:%S", &local_tm);
            printf("Thread %d: Local: %s\n", ta->thread_id, time_str);
        }
        
        // 线程安全的GMT转换
        if (gmtime_r(&now, &gm_tm) != NULL) {
            char time_str[64];
            strftime(time_str, sizeof(time_str),
                     "%Y-%m-%d %H:%M:%S", &gm_tm);
            printf("Thread %d: GMT:   %s\n", ta->thread_id, time_str);
        }
        
        // 小延迟
        struct timespec ts = {0, 10000000};  // 10ms
        nanosleep(&ts, NULL);
    }
    
    return NULL;
}

// 时间分辨率测试
void test_time_resolution(void) {
    printf("\n--- Time Resolution Test ---\n");
    
    struct timespec res;
    if (timespec_getres(TIME_UTC, &res) == TIME_UTC) {
        printf("TIME_UTC resolution:\n");
        printf("  Seconds: %ld\n", (long)res.tv_sec);
        printf("  Nanoseconds: %ld\n", (long)res.tv_nsec);
        
        if (res.tv_sec == 0 && res.tv_nsec == 1) {
            printf("  (1 nanosecond precision)\n");
        } else if (res.tv_sec == 0) {
            printf("  (%.6f millisecond precision)\n", res.tv_nsec / 1e6);
        }
    }
    
    // 测试其他时钟
    #ifdef TIME_MONOTONIC
    if (timespec_getres(TIME_MONOTONIC, &res) == TIME_MONOTONIC) {
        printf("TIME_MONOTONIC resolution: %ld ns\n", (long)res.tv_nsec);
    }
    #endif
}

// 高精度计时器示例
typedef struct {
    struct timespec start;
    struct timespec end;
} Stopwatch;

void stopwatch_start(Stopwatch *sw) {
    timespec_get(&sw->start, TIME_UTC);
}

void stopwatch_stop(Stopwatch *sw) {
    timespec_get(&sw->end, TIME_UTC);
}

double stopwatch_elapsed_ms(const Stopwatch *sw) {
    long sec = sw->end.tv_sec - sw->start.tv_sec;
    long nsec = sw->end.tv_nsec - sw->start.tv_nsec;
    
    if (nsec < 0) {
        sec--;
        nsec += 1000000000L;
    }
    
    return sec * 1000.0 + nsec / 1000000.0;
}

// 性能测试
void performance_test(void) {
    printf("\n--- Performance Test ---\n");
    
    Stopwatch sw;
    const int iterations = 1000000;
    
    // 测试 timespec_get
    stopwatch_start(&sw);
    for (int i = 0; i < iterations; i++) {
        struct timespec ts;
        timespec_get(&ts, TIME_UTC);
    }
    stopwatch_stop(&sw);
    printf("timespec_get (%d calls): %.3f ms\n", iterations, stopwatch_elapsed_ms(&sw));
    
    // 测试 time()
    stopwatch_start(&sw);
    for (int i = 0; i < iterations; i++) {
        time_t t = time(NULL);
    }
    stopwatch_stop(&sw);
    printf("time() (%d calls): %.3f ms\n", iterations, stopwatch_elapsed_ms(&sw));
}

// 格式化时间戳
void print_timestamp(void) {
    struct timespec ts;
    if (timespec_get(&ts, TIME_UTC) == TIME_UTC) {
        struct tm tm_info;
        gmtime_r(&ts.tv_sec, &tm_info);
        
        char buf[128];
        strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &tm_info);
        
        printf("ISO 8601 timestamp: %s.%09ldZ\n", buf, (long)ts.tv_nsec);
    }
}

int main(void) {
    printf("C23 Thread-Safe Time Functions Demo\n");
    printf("====================================\n\n");
    
    test_time_resolution();
    
    printf("\n--- Current Timestamp ---\n");
    print_timestamp();
    
    performance_test();
    
    printf("\n--- Multi-threaded Test ---\n");
    pthread_t threads[3];
    ThreadArg args[3] = {
        {1, 3},
        {2, 3},
        {3, 3}
    };
    
    for (int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, time_worker, &args[i]);
    }
    
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("\nThread-safe time functions demo complete!\n");
    
    return 0;
}
