/*
 * Auto-generated from: 10_WebAssembly_C\03_WASM_Modules_Memory_Management.md
 * Line: 2235
 * Language: c
 * Block ID: 9fc1e9c8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// cache_optimization.c
#include <emscripten/emscripten.h>
#include <stdlib.h>

// 缓存行大小通常为 64 字节
#define CACHE_LINE_SIZE 64

// 结构体布局优化（SoA vs AoS）

// AoS (Array of Structs) - 缓存不友好，用于随机访问
typedef struct {
    float x, y, z;
    float vx, vy, vz;
    int id;
} Particle_AoS;

// SoA (Struct of Arrays) - 缓存友好，用于批量处理
typedef struct {
    float* x, * y, * z;
    float* vx, * vy, * vz;
    int* id;
    int count;
} Particle_SoA;

// SoA 操作 - 更好的缓存局部性
EMSCRIPTEN_KEEPALIVE
void update_particles_soa(Particle_SoA* particles, int count, float dt) {
    // 顺序访问，缓存友好
    for (int i = 0; i < count; i++) {
        particles->x[i] += particles->vx[i] * dt;
        particles->y[i] += particles->vy[i] * dt;
        particles->z[i] += particles->vz[i] * dt;
    }
}

// AoS 操作 - 缓存不友好（访问不连续）
EMSCRIPTEN_KEEPALIVE
void update_particles_aos(Particle_AoS* particles, int count, float dt) {
    for (int i = 0; i < count; i++) {
        particles[i].x += particles[i].vx * dt;
        particles[i].y += particles[i].vy * dt;
        particles[i].z += particles[i].vz * dt;
    }
}

// 预取提示（如果编译器支持）
#ifdef __EMSCRIPTEN__
#define PREFETCH(addr) __builtin_prefetch(addr, 1, 3)
#else
#define PREFETCH(addr)
#endif

EMSCRIPTEN_KEEPALIVE
void prefetch_example(float* data, int n) {
    const int PREFETCH_DISTANCE = 16;

    for (int i = 0; i < n; i++) {
        // 预取未来的数据
        if (i + PREFETCH_DISTANCE < n) {
            PREFETCH(&data[i + PREFETCH_DISTANCE]);
        }

        // 处理当前数据
        data[i] *= 2.0f;
    }
}
