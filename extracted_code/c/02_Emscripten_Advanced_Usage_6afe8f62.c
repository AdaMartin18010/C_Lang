/*
 * Auto-generated from: 10_WebAssembly_C\02_Emscripten_Advanced_Usage.md
 * Line: 2141
 * Language: c
 * Block ID: 6afe8f62
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ========== C 文件操作 ==========

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emscripten/emscripten.h>

// 写入文本文件
EMSCRIPTEN_KEEPALIVE
int write_text_file(const char* filename, const char* content) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Failed to open file for writing: %s\n", filename);
        return -1;
    }

    size_t written = fwrite(content, 1, strlen(content), file);
    fclose(file);

    return (written == strlen(content)) ? 0 : -1;
}

// 读取文本文件
EMSCRIPTEN_KEEPALIVE
char* read_text_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Failed to open file for reading: %s\n", filename);
        return NULL;
    }

    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // 分配缓冲区
    char* buffer = (char*)malloc(size + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }

    // 读取内容
    size_t read = fread(buffer, 1, size, file);
    buffer[read] = '\0';
    fclose(file);

    return buffer;
}

// 写入二进制文件
EMSCRIPTEN_KEEPALIVE
int write_binary_file(const char* filename, const void* data, size_t size) {
    FILE* file = fopen(filename, "wb");
    if (!file) return -1;

    size_t written = fwrite(data, 1, size, file);
    fclose(file);

    return (written == size) ? 0 : -1;
}

// 读取二进制文件
EMSCRIPTEN_KEEPALIVE
void* read_binary_file(const char* filename, size_t* out_size) {
    FILE* file = fopen(filename, "rb");
    if (!file) return NULL;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    void* buffer = malloc(size);
    if (!buffer) {
        fclose(file);
        return NULL;
    }

    *out_size = fread(buffer, 1, size, file);
    fclose(file);

    return buffer;
}

// 追加内容到文件
EMSCRIPTEN_KEEPALIVE
int append_to_file(const char* filename, const char* content) {
    FILE* file = fopen(filename, "a");
    if (!file) return -1;

    fprintf(file, "%s", content);
    fclose(file);
    return 0;
}

// 检查文件是否存在
EMSCRIPTEN_KEEPALIVE
int file_exists(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

// 获取文件大小
EMSCRIPTEN_KEEPALIVE
long get_file_size(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return -1;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);
    return size;
}

// 删除文件
EMSCRIPTEN_KEEPALIVE
int delete_file(const char* filename) {
    return remove(filename);
}

// 创建目录
EMSCRIPTEN_KEEPALIVE
int create_directory(const char* path) {
    #ifdef _WIN32
        return _mkdir(path);
    #else
        return mkdir(path, 0755);
    #endif
}

// 列出目录内容
EMSCRIPTEN_KEEPALIVE
void list_directory(const char* path) {
    DIR* dir = opendir(path);
    if (!dir) {
        fprintf(stderr, "Failed to open directory: %s\n", path);
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
}

// CSV 文件处理示例
EMSCRIPTEN_KEEPALIVE
int process_csv_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return -1;

    char line[1024];
    int row = 0;

    while (fgets(line, sizeof(line), file)) {
        // 解析 CSV 行
        char* token = strtok(line, ",");
        int col = 0;

        while (token) {
            printf("Row %d, Col %d: %s\n", row, col, token);
            token = strtok(NULL, ",");
            col++;
        }
        row++;
    }

    fclose(file);
    return row;
}
