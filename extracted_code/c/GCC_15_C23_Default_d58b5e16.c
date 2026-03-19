/*
 * Auto-generated from: 00_VERSION_TRACKING\GCC_15_C23_Default.md
 * Line: 1092
 * Language: c
 * Block ID: d58b5e16
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* timespec_getres 示例 */
#include <stdio.h>
#include <time.h>

int main(void) {
    struct timespec res;

    /* 获取实时时钟的分辨率 */
    int result = timespec_getres(&res, TIME_UTC);

    if (result == TIME_UTC) {
        printf("TIME_UTC resolution: %ld seconds, %ld nanoseconds\n",
               (long)res.tv_sec, res.tv_nsec);
        printf("Resolution: %.9f seconds\n",
               res.tv_sec + res.tv_nsec / 1e9);
    } else {
        printf("Failed to get resolution\n");
    }

    /* 获取单调时钟的分辨率 (如果支持) */
    #ifdef TIME_MONOTONIC
    result = timespec_getres(&res, TIME_MONOTONIC);
    if (result == TIME_MONOTONIC) {
        printf("TIME_MONOTONIC resolution: %.9f seconds\n",
               res.tv_sec + res.tv_nsec / 1e9);
    }
    #endif

    return 0;
}
