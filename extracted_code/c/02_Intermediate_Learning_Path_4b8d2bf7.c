/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\02_Intermediate_Learning_Path.md
 * Line: 370
 * Language: c
 * Block ID: 4b8d2bf7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// mycp.c - 支持多种复制模式的文件复制工具
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

// 复制模式
typedef enum {
    MODE_SIMPLE,     // 简单读写
    MODE_BUFFERED,   // 缓冲复制
    MODE_MMAP,       // 内存映射
    MODE_SENDFILE    // 零拷贝
} CopyMode;

// 功能要求：
// 1. 支持4种复制模式，可命令行选择
// 2. 显示复制速度和进度
// 3. 支持大文件（>4GB）
// 4. 支持目录递归复制
// 5. 保留文件属性

int copy_file(const char *src, const char *dst, CopyMode mode);
int copy_directory(const char *src, const char *dst, CopyMode mode);
void show_progress(off_t current, off_t total);
