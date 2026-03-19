/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\09_Performance_Optimization.md
 * Line: 39
 * Language: c
 * Block ID: 86566e2d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Linux: 使用clock_gettime
#include <time.h>

struct timespec start, end;
clock_gettime(CLOCK_MONOTONIC, &start);

// 被测代码
function_to_measure();

clock_gettime(CLOCK_MONOTONIC, &end);
double elapsed = (end.tv_sec - start.tv_sec) +
                 (end.tv_nsec - start.tv_nsec) / 1e9;

// 或者使用RDTSC (x86)
#include <x86intrin.h>

uint64_t start = __rdtsc();
// 被测代码
uint64_t end = __rdtsc();
uint64_t cycles = end - start;
