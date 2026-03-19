/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\05_Cache_Optimization_Real_World.md
 * Line: 364
 * Language: c
 * Block ID: c638b3b8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Array of Structures (AoS) - 缓存不友好
struct ParticleAoS {
    float x, y, z;      // 位置
    float vx, vy, vz;   // 速度
    float mass;         // 质量
    int id;             // ID
};  // 32字节

struct ParticleAoS particles_aos[N];

// 只更新位置时的访问模式
for (int i = 0; i < N; i++) {
    particles_aos[i].x += particles_aos[i].vx * dt;  // 加载32字节，使用4字节
    particles_aos[i].y += particles_aos[i].vy * dt;  // 缓存行已失效
    particles_aos[i].z += particles_aos[i].vz * dt;  // 重新加载
}
// 缓存效率: 4/32 = 12.5%

// Structure of Arrays (SoA) - 缓存友好
struct ParticleSoA {
    float *x, *y, *z;
    float *vx, *vy, *vz;
    float *mass;
    int *id;
};

struct ParticleSoA particles_soa;

// 分配连续数组
particles_soa.x = aligned_alloc(64, N * sizeof(float));
// ... 分配其他数组

// 更新位置
for (int i = 0; i < N; i++) {
    particles_soa.x[i] += particles_soa.vx[i] * dt;  // 连续访问
}
for (int i = 0; i < N; i++) {
    particles_soa.y[i] += particles_soa.vy[i] * dt;  // 连续访问
}
for (int i = 0; i < N; i++) {
    particles_soa.z[i] += particles_soa.vz[i] * dt;  // 连续访问
}
// 缓存效率: 100%

/* 性能对比 (N=1M粒子):
 * AoS:  12.5 ms
 * SoA:  2.1 ms  (6x加速)
 */
