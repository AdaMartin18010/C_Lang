/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_If_Declaration_Depth.md
 * Line: 742
 * Language: c
 * Block ID: 8c8d4829
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 示例 2: 文件操作中的 if 声明
 * 演示资源获取、错误处理和作用域控制
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

// 简单的文件复制
bool copy_file_simple(const char* src, const char* dst) {
    if (FILE* src_fp = fopen(src, "rb"); src_fp == NULL) {
        fprintf(stderr, "Error: Cannot open source file '%s'\n", src);
        return false;
    } else {
        // src_fp 在此作用域内有效
        if (FILE* dst_fp = fopen(dst, "wb"); dst_fp == NULL) {
            fprintf(stderr, "Error: Cannot open destination file '%s'\n", dst);
            fclose(src_fp);
            return false;
        } else {
            // 两个文件指针都可用
            char buffer[BUFFER_SIZE];
            size_t bytes_read;
            size_t total_copied = 0;

            while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, src_fp)) > 0) {
                if (fwrite(buffer, 1, bytes_read, dst_fp) != bytes_read) {
                    fprintf(stderr, "Error: Write failed\n");
                    fclose(dst_fp);
                    fclose(src_fp);
                    return false;
                }
                total_copied += bytes_read;
            }

            printf("Copied %zu bytes\n", total_copied);
            fclose(dst_fp);
        }
        fclose(src_fp);
    }
    return true;
}

// 读取整个文件到内存
char* read_file_contents(const char* filename, size_t* out_size) {
    if (FILE* fp = fopen(filename, "rb"); fp == NULL) {
        perror("Failed to open file");
        return NULL;
    } else {
        // 获取文件大小
        if (fseek(fp, 0, SEEK_END) != 0) {
            perror("Failed to seek");
            fclose(fp);
            return NULL;
        }

        if (long file_size = ftell(fp); file_size < 0) {
            perror("Failed to get file size");
            fclose(fp);
            return NULL;
        } else {
            rewind(fp);

            if (char* contents = malloc(file_size + 1); contents == NULL) {
                perror("Failed to allocate memory");
                fclose(fp);
                return NULL;
            } else {
                if (size_t read_size = fread(contents, 1, file_size, fp);
                    read_size != (size_t)file_size) {
                    fprintf(stderr, "Warning: Read %zu bytes, expected %ld\n",
                            read_size, file_size);
                }

                contents[file_size] = '\0';
                *out_size = file_size;

                fclose(fp);
                return contents;
            }
        }
    }
}

// 安全的日志写入
void log_message(const char* logfile, const char* message) {
    if (FILE* fp = fopen(logfile, "a"); fp == NULL) {
        // 无法打开日志文件，输出到 stderr
        fprintf(stderr, "[LOG ERROR] %s\n", message);
    } else {
        time_t now = time(NULL);
        struct tm* tm_info = localtime(&now);
        char timestamp[26];
        strftime(timestamp, 26, "%Y-%m-%d %H:%M:%S", tm_info);

        fprintf(fp, "[%s] %s\n", timestamp, message);
        fclose(fp);
    }
}

int main(void) {
    printf("=== File Operations Demo ===\n");

    // 创建测试文件
    if (FILE* fp = fopen("test_input.txt", "w"); fp != NULL) {
        fprintf(fp, "Hello, C2y if declaration!\n");
        fprintf(fp, "This is a test file.\n");
        fclose(fp);
    }

    // 复制文件
    if (copy_file_simple("test_input.txt", "test_output.txt")) {
        printf("File copied successfully!\n");
    }

    // 读取文件内容
    size_t size;
    if (char* contents = read_file_contents("test_output.txt", &size);
        contents != NULL) {
        printf("File contents:\n%s\n", contents);
        free(contents);
    }

    // 记录日志
    log_message("test.log", "Test message from C2y if declaration demo");

    // 清理
    remove("test_input.txt");
    remove("test_output.txt");
    remove("test.log");

    return 0;
}
