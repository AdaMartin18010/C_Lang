/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\README.md
 * Line: 1154
 * Language: c
 * Block ID: c4d31597
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// storage_interface.h - 纯接口定义
#ifndef STORAGE_INTERFACE_H
#define STORAGE_INTERFACE_H

#include <stddef.h>

typedef struct Storage Storage;

typedef struct {
    int (*open)(Storage* self, const char* path);
    int (*read)(Storage* self, void* buf, size_t size);
    int (*write)(Storage* self, const void* buf, size_t size);
    int (*close)(Storage* self);
    void (*destroy)(Storage* self);
} StorageInterface;

struct Storage {
    const StorageInterface* vtable;
};

// 辅助内联函数
static inline int storage_open(Storage* s, const char* p) {
    return s->vtable->open(s, p);
}
static inline int storage_read(Storage* s, void* b, size_t n) {
    return s->vtable->read(s, b, n);
}
// ...

#endif
