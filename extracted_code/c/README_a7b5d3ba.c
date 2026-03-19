/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\README.md
 * Line: 1226
 * Language: c
 * Block ID: a7b5d3ba
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// memory_storage.c - 内存存储实现
typedef struct {
    Storage base;
    char* buffer;
    size_t size;
    size_t pos;
} MemoryStorage;

static int mem_write(Storage* self, const void* buf, size_t size) {
    MemoryStorage* ms = (MemoryStorage*)self;
    if (ms->pos + size > ms->size) return -1;
    memcpy(ms->buffer + ms->pos, buf, size);
    ms->pos += size;
    return size;
}
// ... 其他实现

Storage* memory_storage_create(void* buffer, size_t size) {
    MemoryStorage* ms = calloc(1, sizeof(MemoryStorage));
    ms->base.vtable = &memory_storage_vtable;
    ms->buffer = buffer;
    ms->size = size;
    return (Storage*)ms;
}
