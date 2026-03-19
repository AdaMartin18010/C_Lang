/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\README.md
 * Line: 153
 * Language: c
 * Block ID: 97600e85
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef ZIG_ALLOC_H
#define ZIG_ALLOC_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int zig_alloc_init(void);
void zig_alloc_deinit(void);
void* zig_malloc(size_t size);
void zig_free(void* ptr, size_t size);

#ifdef __cplusplus
}
#endif

#endif // ZIG_ALLOC_H
