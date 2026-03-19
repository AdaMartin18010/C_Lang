/*
 * Auto-generated from: 06_Thinking_Representation\01_Decision_Trees\03_Performance_Bottleneck.md
 * Line: 239
 * Language: c
 * Block ID: 779b0aa0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// io_bound.c - IO密集型任务示例
#include <stdio.h>
#include <stdlib.h>
#include "benchmark.h"

#define BUFFER_SIZE 4096
#define FILE_SIZE (1024 * 1024 * 10)  // 10MB

// 低效的逐字节IO
void inefficient_io_write(const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) return;

    for (int i = 0; i < FILE_SIZE; i++) {
        char c = (char)(i % 256);
        fwrite(&c, 1, 1, fp);  // 每次写入1字节
    }
    fclose(fp);
}

// 优化: 缓冲写入
void optimized_io_write(const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) return;

    char buffer[BUFFER_SIZE];
    for (int i = 0; i < FILE_SIZE; i += BUFFER_SIZE) {
        for (int j = 0; j < BUFFER_SIZE && i + j < FILE_SIZE; j++) {
            buffer[j] = (char)((i + j) % 256);
        }
        fwrite(buffer, 1,
               (i + BUFFER_SIZE <= FILE_SIZE) ? BUFFER_SIZE : (FILE_SIZE - i),
               fp);
    }
    fclose(fp);
}

// 使用setvbuf优化
void buffered_io_write(const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) return;

    // 设置大缓冲区
    char *buf = malloc(BUFFER_SIZE * 16);
    setvbuf(fp, buf, _IOFBF, BUFFER_SIZE * 16);

    for (int i = 0; i < FILE_SIZE; i++) {
        char c = (char)(i % 256);
        fwrite(&c, 1, 1, fp);
    }

    fclose(fp);
    free(buf);
}

int main(void) {
    printf("=== IO Bound Test ===\n");
    BENCHMARK(inefficient_io_write("test1.bin"), 1);
    BENCHMARK(optimized_io_write("test2.bin"), 1);
    BENCHMARK(buffered_io_write("test3.bin"), 1);

    // 清理
    remove("test1.bin");
    remove("test2.bin");
    remove("test3.bin");

    return 0;
}
