/*
 * Auto-generated from: 06_Thinking_Representation\01_Decision_Trees\03_Performance_Bottleneck.md
 * Line: 349
 * Language: c
 * Block ID: 0951ef96
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// gprof_example.c - 用于gprof分析的示例
#include <stdio.h>
#include <stdlib.h>

void slow_function(int n) {
    double sum = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            sum += i * j * 0.5;
        }
    }
    (void)sum;  // 抑制未使用警告
}

void medium_function(int n) {
    double sum = 0;
    for (int i = 0; i < n; i++) {
        sum += i * i;
    }
    (void)sum;
}

void fast_function(int n) {
    printf("Processing %d items\n", n);
}

int main(int argc, char *argv[]) {
    int n = (argc > 1) ? atoi(argv[1]) : 1000;

    for (int i = 0; i < 10; i++) {
        fast_function(n);
        medium_function(n);
        slow_function(n);
    }

    return 0;
}
