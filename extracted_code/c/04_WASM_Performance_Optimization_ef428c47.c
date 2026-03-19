/*
 * Auto-generated from: 10_WebAssembly_C\04_WASM_Performance_Optimization.md
 * Line: 1342
 * Language: c
 * Block ID: ef428c47
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// profiler.c
#include <emscripten/emscripten.h>
#include <stdio.h>
#include <string.h>

#define MAX_PROFILE_POINTS 100

typedef struct {
    const char* name;
    double start_time;
    double total_time;
    int call_count;
    int active;
} ProfilePoint;

static ProfilePoint profiles[MAX_PROFILE_POINTS];
static int profile_count = 0;

EMSCRIPTEN_KEEPALIVE
void profile_start(const char* name) {
    // 查找或创建记录点
    int idx = -1;
    for (int i = 0; i < profile_count; i++) {
        if (strcmp(profiles[i].name, name) == 0) {
            idx = i;
            break;
        }
    }

    if (idx < 0 && profile_count < MAX_PROFILE_POINTS) {
        idx = profile_count++;
        profiles[idx].name = name;
        profiles[idx].total_time = 0;
        profiles[idx].call_count = 0;
        profiles[idx].active = 0;
    }

    if (idx >= 0 && !profiles[idx].active) {
        profiles[idx].start_time = emscripten_get_now();
        profiles[idx].active = 1;
    }
}

EMSCRIPTEN_KEEPALIVE
void profile_end(const char* name) {
    double end_time = emscripten_get_now();

    for (int i = 0; i < profile_count; i++) {
        if (strcmp(profiles[i].name, name) == 0 && profiles[i].active) {
            profiles[i].total_time += end_time - profiles[i].start_time;
            profiles[i].call_count++;
            profiles[i].active = 0;
            return;
        }
    }
}

EMSCRIPTEN_KEEPALIVE
void profile_report() {
    printf("\n=== 性能分析报告 ===\n\n");
    printf("%-30s %10s %10s %10s\n", "名称", "总时间(ms)", "调用次数", "平均(ms)");
    printf("%-30s %10s %10s %10s\n", "------------------------------",
           "----------", "----------", "----------");

    for (int i = 0; i < profile_count; i++) {
        double avg = profiles[i].call_count > 0 ?
            profiles[i].total_time / profiles[i].call_count : 0;
        printf("%-30s %10.3f %10d %10.3f\n",
               profiles[i].name,
               profiles[i].total_time,
               profiles[i].call_count,
               avg);
    }
}

// 宏定义便于使用
#define PROFILE_SCOPE(name) \
    profile_start(name); \
    for (int _profile_i = 0; _profile_i < 1; _profile_i++, profile_end(name))

// 使用示例
void optimized_function() {
    PROFILE_SCOPE("optimized_function") {
        // 函数代码
        for (int i = 0; i < 1000; i++) {
            PROFILE_SCOPE("inner_loop") {
                // 循环代码
            }
        }
    }
}
