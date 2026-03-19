/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\07_Microarchitecture\01_Cycle_Accurate_Semantics.md
 * Line: 158
 * Language: c
 * Block ID: 93a717a1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 数组结构体 (AoS)：访问特定字段时加载无用数据
typedef struct {
    float x, y, z;      // 位置
    float vx, vy, vz;   // 速度
    float mass;         // 质量
    int id;             // ID
    // 对齐到64字节
    char pad[64 - 28];
} Particle;

Particle particles[10000];

// 更新位置：每次只读vx,vy,vz，但加载整个64字节
void update_aos(void) {
    for (int i = 0; i < 10000; i++) {
        particles[i].x += particles[i].vx;
        particles[i].y += particles[i].vy;
        particles[i].z += particles[i].vz;
        // 加载64字节，使用12字节，浪费81%
    }
}

// ✅ 结构体数组 (SoA)：数据分离，提高局部性
typedef struct {
    float *x, *y, *z;
    float *vx, *vy, *vz;
} ParticleSystem;

void update_soa(ParticleSystem *ps, int n) {
    for (int i = 0; i < n; i++) {
        ps->x[i] += ps->vx[i];
        ps->y[i] += ps->vy[i];
        ps->z[i] += ps->vz[i];
        // 顺序访问，缓存行完全利用
    }
}
