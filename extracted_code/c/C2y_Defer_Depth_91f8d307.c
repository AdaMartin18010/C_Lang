/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 1423
 * Language: c
 * Block ID: 91f8d307
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// file: error_handling.c
// compile: clang -std=c2y error_handling.c -o error_handling

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

// 错误码定义
typedef enum {
    ERR_OK = 0,
    ERR_NULL_PTR = -1,
    ERR_ALLOC = -2,
    ERR_FILE = -3,
    ERR_INVALID = -4,
} ErrorCode;

// 上下文结构体
typedef struct {
    FILE *input;
    FILE *output;
    char *buffer;
    size_t buffer_size;
} ProcessingContext;

const char *error_code_to_string(ErrorCode code) {
    switch (code) {
        case ERR_OK: return "Success";
        case ERR_NULL_PTR: return "Null pointer";
        case ERR_ALLOC: return "Memory allocation failed";
        case ERR_FILE: return "File operation failed";
        case ERR_INVALID: return "Invalid parameter";
        default: return "Unknown error";
    }
}

ErrorCode process_files(const char *input_path, const char *output_path) {
    ProcessingContext ctx = {0};

    // 参数检查
    if (!input_path || !output_path) {
        return ERR_NULL_PTR;
    }

    // 打开输入文件
    ctx.input = fopen(input_path, "r");
    if (!ctx.input) {
        fprintf(stderr, "Cannot open input file '%s': %s\n",
                input_path, strerror(errno));
        return ERR_FILE;
    }
    defer {
        printf("Cleanup: closing input file\n");
        if (ctx.input) fclose(ctx.input);
    }

    // 打开输出文件
    ctx.output = fopen(output_path, "w");
    if (!ctx.output) {
        fprintf(stderr, "Cannot open output file '%s': %s\n",
                output_path, strerror(errno));
        return ERR_FILE;
    }
    defer {
        printf("Cleanup: closing output file\n");
        if (ctx.output) fclose(ctx.output);
    }

    // 分配缓冲区
    ctx.buffer_size = 4096;
    ctx.buffer = malloc(ctx.buffer_size);
    if (!ctx.buffer) {
        fprintf(stderr, "Memory allocation failed\n");
        return ERR_ALLOC;
    }
    defer {
        printf("Cleanup: freeing buffer\n");
        if (ctx.buffer) free(ctx.buffer);
    }

    // 处理文件内容
    printf("Processing started...\n");

    size_t bytes_read;
    size_t total_processed = 0;

    while ((bytes_read = fread(ctx.buffer, 1, ctx.buffer_size, ctx.input)) > 0) {
        // 处理数据（这里只是简单地复制）
        if (fwrite(ctx.buffer, 1, bytes_read, ctx.output) != bytes_read) {
            fprintf(stderr, "Write error\n");
            return ERR_FILE;
        }
        total_processed += bytes_read;
    }

    if (ferror(ctx.input)) {
        fprintf(stderr, "Read error\n");
        return ERR_FILE;
    }

    printf("Processing completed: %zu bytes processed\n", total_processed);

    // 确保数据写入磁盘
    if (fflush(ctx.output) != 0) {
        fprintf(stderr, "Flush error\n");
        return ERR_FILE;
    }

    return ERR_OK;  // 所有资源自动清理
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);

        // 创建测试文件
        FILE *test = fopen("test_input.txt", "w");
        if (test) {
            fprintf(test, "This is a test file for C2y defer demonstration.\n");
            fprintf(test, "It contains multiple lines of text.\n");
            fprintf(test, "Line 3: Hello, C2y defer!\n");
            fclose(test);
            printf("Created test_input.txt\n");
        }

        argv[1] = "test_input.txt";
        argv[2] = "test_output.txt";
    }

    ErrorCode result = process_files(argv[1], argv[2]);

    printf("\nResult: %s (%d)\n", error_code_to_string(result), result);

    return result == ERR_OK ? 0 : 1;
}
