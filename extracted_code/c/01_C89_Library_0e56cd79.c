/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\01_C89_Library.md
 * Line: 158
 * Language: c
 * Block ID: 0e56cd79
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ✅ 安全文件读取模式
#include <stdio.h>
#include <stdlib.h>

// 读取整个文件到缓冲区
char *read_file(const char *filename, size_t *out_size) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) return NULL;

    // 获取文件大小
    if (fseek(fp, 0, SEEK_END) != 0) goto error;
    long size = ftell(fp);
    if (size < 0) goto error;
    if (fseek(fp, 0, SEEK_SET) != 0) goto error;

    // 分配缓冲区（+1 for null terminator if text）
    char *buffer = malloc(size + 1);
    if (!buffer) goto error;

    // 读取
    size_t read = fread(buffer, 1, size, fp);
    if (read != (size_t)size) {
        free(buffer);
        goto error;
    }
    buffer[size] = '\0';

    fclose(fp);
    if (out_size) *out_size = size;
    return buffer;

error:
    fclose(fp);
    return NULL;
}

// ✅ 安全写入
typedef enum { FILE_OK, FILE_ERROR, FILE_TRUNCATED } FileResult;

FileResult write_file(const char *filename, const void *data, size_t size) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) return FILE_ERROR;

    size_t written = fwrite(data, 1, size, fp);
    int flush_ok = (fflush(fp) == 0);
    int close_ok = (fclose(fp) == 0);

    if (written != size) return FILE_TRUNCATED;
    if (!flush_ok || !close_ok) return FILE_ERROR;
    return FILE_OK;
}
