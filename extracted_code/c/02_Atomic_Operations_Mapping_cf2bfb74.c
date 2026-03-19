/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
 * Line: 973
 * Language: c
 * Block ID: cf2bfb74
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 缓存行填充
#define CACHE_LINE_SIZE 64

typedef struct {
    _Atomic uint64_t count;
    char padding[CACHE_LINE_SIZE - sizeof(_Atomic uint64_t)];
} PaddedCounter;
