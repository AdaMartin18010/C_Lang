/*
 * Auto-generated from: 10_WebAssembly_C\04_WASM_Performance_Optimization.md
 * Line: 159
 * Language: c
 * Block ID: aa3fd699
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// data_structure_optimization.c
#include <emscripten/emscripten.h>
#include <stdlib.h>

// ========== AoS (Array of Structs) vs SoA (Struct of Arrays) ==========

// AoS - 缓存不友好，但易于使用
typedef struct {
    float x, y, z;
    float vx, vy, vz;
    int id;
    float mass;
    int active;
} Particle_AoS;

// SoA - 缓存友好，向量化友好
typedef struct {
    float* x, * y, * z;
    float* vx, * vy, * vz;
    int* id;
    float* mass;
    int* active;
    int count;
} Particle_SoA;

// SoA 批量更新 - 更好的性能
EMSCRIPTEN_KEEPALIVE
void update_particles_soa(Particle_SoA* particles, int n, float dt) {
    // 连续访问 x 坐标
    for (int i = 0; i < n; i++) {
        particles->x[i] += particles->vx[i] * dt;
    }
    // 连续访问 y 坐标
    for (int i = 0; i < n; i++) {
        particles->y[i] += particles->vy[i] * dt;
    }
    // 连续访问 z 坐标
    for (int i = 0; i < n; i++) {
        particles->z[i] += particles->vz[i] * dt;
    }
}

// ========== 内存对齐 ==========

// 不对齐的结构体
typedef struct {
    char a;      // 1 byte + 3 padding
    int b;       // 4 bytes
    char c;      // 1 byte + 3 padding
    double d;    // 8 bytes
} UnalignedStruct;  // 总大小: 24 bytes

// 对齐的结构体
typedef struct {
    double d;    // 8 bytes
    int b;       // 4 bytes
    char a;      // 1 byte
    char c;      // 1 byte + 2 padding
} AlignedStruct;  // 总大小: 16 bytes

// 强制对齐
typedef struct {
    float data[4];
} __attribute__((aligned(16))) AlignedVec4;

// ========== 热点数据结构 ==========

// 缓存友好的哈希表（线性探测）
#define HASH_SIZE 1024
#define HASH_MASK (HASH_SIZE - 1)

typedef struct {
    int key;
    int value;
    int occupied;
} HashEntry;

static HashEntry hash_table[HASH_SIZE];

EMSCRIPTEN_KEEPALIVE
void hash_insert(int key, int value) {
    int idx = key & HASH_MASK;
    while (hash_table[idx].occupied) {
        if (hash_table[idx].key == key) {
            hash_table[idx].value = value;
            return;
        }
        idx = (idx + 1) & HASH_MASK;
    }
    hash_table[idx].key = key;
    hash_table[idx].value = value;
    hash_table[idx].occupied = 1;
}

EMSCRIPTEN_KEEPALIVE
int hash_lookup(int key, int* found) {
    int idx = key & HASH_MASK;
    while (hash_table[idx].occupied) {
        if (hash_table[idx].key == key) {
            *found = 1;
            return hash_table[idx].value;
        }
        idx = (idx + 1) & HASH_MASK;
    }
    *found = 0;
    return 0;
}
