/*
 * Auto-generated from: 00_VERSION_TRACKING\GCC_15_C23_Default.md
 * Line: 1127
 * Language: c
 * Block ID: eed9d379
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C23 高精度计时器 */
#include <stdio.h>
#include <time.h>

/* 计时器结构体 */
typedef struct {
    struct timespec start;
    struct timespec end;
    struct timespec resolution;
    const char* name;
} timer_t;

/* 初始化计时器 */
void timer_init(timer_t* timer, const char* name) {
    timer->name = name;
    timespec_getres(&timer->resolution, TIME_UTC);
}

/* 开始计时 */
void timer_start(timer_t* timer) {
    timespec_get(&timer->start, TIME_UTC);
}

/* 停止计时 */
void timer_stop(timer_t* timer) {
    timespec_get(&timer->end, TIME_UTC);
}

/* 获取经过的时间（纳秒） */
long long timer_elapsed_ns(timer_t* timer) {
    long long sec = timer->end.tv_sec - timer->start.tv_sec;
    long long nsec = timer->end.tv_nsec - timer->start.tv_nsec;
    return sec * 1000000000LL + nsec;
}

/* 获取经过的时间（秒） */
double timer_elapsed(timer_t* timer) {
    return timer_elapsed_ns(timer) / 1e9;
}

/* 打印计时结果 */
void timer_print(timer_t* timer) {
    double elapsed = timer_elapsed(timer);
    double resolution = timer->resolution.tv_sec +
                        timer->resolution.tv_nsec / 1e9;

    printf("Timer '%s':\n", timer->name);
    printf("  Resolution: %.9f sec\n", resolution);
    printf("  Elapsed: %.9f sec\n", elapsed);
    printf("  Elapsed: %.6f ms\n", elapsed * 1000);
    printf("  Elapsed: %.3f us\n", elapsed * 1000000);
    printf("  Elapsed: %.0f ns\n", elapsed * 1000000000);
}

/* 使用示例 */
void example_usage(void) {
    timer_t timer;
    timer_init(&timer, "Matrix Multiplication");

    timer_start(&timer);

    /* 模拟工作 */
    volatile long long sum = 0;
    for (int i = 0; i < 1000000; i++) {
        sum += i;
    }

    timer_stop(&timer);
    timer_print(&timer);
}

int main(void) {
    example_usage();
    return 0;
}
