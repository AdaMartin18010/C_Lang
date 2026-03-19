/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\02_Pointer_Deep_Dive.md
 * Line: 762
 * Language: c
 * Block ID: 0ce44a1a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// header.h - 只声明不透明类型
typedef struct FileHandle FileHandle;
FileHandle* file_open(const char* path);
void file_close(FileHandle* fh);

// impl.c - 定义实际结构
struct FileHandle {
    int fd;
    char buffer[1024];
    // ... 内部实现细节
};
