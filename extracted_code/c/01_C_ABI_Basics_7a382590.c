/*
 * Auto-generated from: 03_System_Technology_Domains\16_Rust_Interoperability\01_C_ABI_Basics.md
 * Line: 110
 * Language: c
 * Block ID: 7a382590
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// === opaque.c ===
#include <stdlib.h>
#include <string.h>

// 隐藏实现细节
typedef struct {
    int *data;
    size_t len;
    size_t pos;
} InternalIterator;

// 构造函数
void* iterator_create(const int *data, size_t len) {
    InternalIterator *it = malloc(sizeof(InternalIterator));
    if (!it) return NULL;

    it->data = malloc(len * sizeof(int));
    if (!it->data) {
        free(it);
        return NULL;
    }
    memcpy(it->data, data, len * sizeof(int));
    it->len = len;
    it->pos = 0;

    return it;
}

bool iterator_next(void *handle, int *out) {
    InternalIterator *it = handle;
    if (it->pos >= it->len) return false;
    *out = it->data[it->pos++];
    return true;
}

void iterator_destroy(void *handle) {
    InternalIterator *it = handle;
    free(it->data);
    free(it);
}
