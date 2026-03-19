/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\README.md
 * Line: 1187
 * Language: c
 * Block ID: b2f1fc24
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// file_storage.c - 文件存储实现
#include "storage_interface.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    Storage base;
    FILE* fp;
} FileStorage;

static int file_open(Storage* self, const char* path) {
    FileStorage* fs = (FileStorage*)self;
    fs->fp = fopen(path, "r+");
    return fs->fp ? 0 : -1;
}

static int file_read(Storage* self, void* buf, size_t size) {
    FileStorage* fs = (FileStorage*)self;
    return fread(buf, 1, size, fs->fp);
}

// ... 其他实现

static const StorageInterface file_storage_vtable = {
    .open = file_open,
    .read = file_read,
    .write = file_write,
    .close = file_close,
    .destroy = file_destroy
};

Storage* file_storage_create(void) {
    FileStorage* fs = calloc(1, sizeof(FileStorage));
    fs->base.vtable = &file_storage_vtable;
    return (Storage*)fs;
}
