/*
 * Auto-generated from: 10_WebAssembly_C\03_WASM_Modules_Memory_Management.md
 * Line: 2366
 * Language: c
 * Block ID: b6e6aa96
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// game_memory_system.c
#include <emscripten/emscripten.h>
#include <stdlib.h>
#include <string.h>

// 游戏对象内存系统
#define MAX_GAME_OBJECTS 10000
#define GAME_OBJECT_SIZE 256

// 内存区域定义
typedef enum {
    MEM_PERMAMENT,    // 永久存储（关卡数据）
    MEM_LEVEL,        // 关卡生命周期
    MEM_TEMPORARY,    // 临时分配（每帧清理）
    MEM_COUNT
} MemoryZone;

// 内存堆管理器
typedef struct {
    char* base;
    size_t size;
    size_t used;
    int mark;
} MemoryHeap;

static MemoryHeap heaps[MEM_COUNT];

// 初始化内存系统
EMSCRIPTEN_KEEPALIVE
void memory_system_init() {
    // 永久存储: 64MB
    heaps[MEM_PERMAMENT].size = 64 * 1024 * 1024;
    heaps[MEM_PERMAMENT].base = (char*)malloc(heaps[MEM_PERMAMENT].size);
    heaps[MEM_PERMAMENT].used = 0;

    // 关卡存储: 128MB
    heaps[MEM_LEVEL].size = 128 * 1024 * 1024;
    heaps[MEM_LEVEL].base = (char*)malloc(heaps[MEM_LEVEL].size);
    heaps[MEM_LEVEL].used = 0;

    // 临时存储: 32MB
    heaps[MEM_TEMPORARY].size = 32 * 1024 * 1024;
    heaps[MEM_TEMPORARY].base = (char*)malloc(heaps[MEM_TEMPORARY].size);
    heaps[MEM_TEMPORARY].used = 0;
}

// 区域分配
EMSCRIPTEN_KEEPALIVE
void* zone_alloc(MemoryZone zone, size_t size, size_t alignment) {
    if (zone >= MEM_COUNT) return NULL;

    MemoryHeap* heap = &heaps[zone];

    // 对齐
    size_t aligned_used = (heap->used + alignment - 1) & ~(alignment - 1);

    if (aligned_used + size > heap->size) {
        return NULL;  // 内存不足
    }

    void* ptr = heap->base + aligned_used;
    heap->used = aligned_used + size;

    return ptr;
}

// 重置区域
EMSCRIPTEN_KEEPALIVE
void zone_reset(MemoryZone zone) {
    if (zone < MEM_COUNT) {
        heaps[zone].used = 0;
    }
}

// 每帧清理临时内存
EMSCRIPTEN_KEEPALIVE
void frame_end() {
    zone_reset(MEM_TEMPORARY);
}

// 游戏对象池
typedef struct GameObject {
    int id;
    float x, y, z;
    float rotation;
    int active;
    // ... 更多字段
} GameObject;

static GameObject* object_pool = NULL;
static int* free_list = NULL;
static int free_count = 0;
static int next_id = 1;

EMSCRIPTEN_KEEPALIVE
void object_pool_init() {
    object_pool = (GameObject*)zone_alloc(MEM_PERMAMENT,
        sizeof(GameObject) * MAX_GAME_OBJECTS, 64);
    free_list = (int*)zone_alloc(MEM_PERMAMENT,
        sizeof(int) * MAX_GAME_OBJECTS, 64);

    // 初始化空闲列表
    for (int i = 0; i < MAX_GAME_OBJECTS; i++) {
        free_list[i] = MAX_GAME_OBJECTS - 1 - i;
    }
    free_count = MAX_GAME_OBJECTS;
}

EMSCRIPTEN_KEEPALIVE
GameObject* object_create() {
    if (free_count == 0) return NULL;

    int index = free_list[--free_count];
    GameObject* obj = &object_pool[index];

    memset(obj, 0, sizeof(GameObject));
    obj->id = next_id++;
    obj->active = 1;

    return obj;
}

EMSCRIPTEN_KEEPALIVE
void object_destroy(GameObject* obj) {
    if (!obj || !obj->active) return;

    int index = obj - object_pool;
    if (index < 0 || index >= MAX_GAME_OBJECTS) return;

    obj->active = 0;
    free_list[free_count++] = index;
}
