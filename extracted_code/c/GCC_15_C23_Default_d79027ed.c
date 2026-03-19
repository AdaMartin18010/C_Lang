/*
 * Auto-generated from: 00_VERSION_TRACKING\GCC_15_C23_Default.md
 * Line: 881
 * Language: c
 * Block ID: d79027ed
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 精确控制 glibc 特性 */

/* 方案 1: 严格 C23 */
#define _ISOC23_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* 方案 2: C23 + POSIX.1-2024 */
#define _POSIX_C_SOURCE 202405L
#define _ISOC23_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <time.h>

/* 方案 3: 完整功能 (GNU + POSIX + C2y) */
#define _GNU_SOURCE
#define _ISOC2Y_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 特性检测 */
#ifdef _ISOC23_SOURCE
    #define USING_C23 1
#else
    #define USING_C23 0
#endif

#ifdef _ISOC2Y_SOURCE
    #define USING_C2Y 1
#else
    #define USING_C2Y 0
#endif

int main(void) {
    printf("C23 support: %s\n", USING_C23 ? "enabled" : "disabled");
    printf("C2y support: %s\n", USING_C2Y ? "enabled" : "disabled");

    return 0;
}
