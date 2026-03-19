/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\02_Cache_Line_Optimization.md
 * Line: 217
 * Language: c
 * Block ID: 3a4de9c8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// AoS - Array of Structs (面向对象风格)
struct ParticleAoS {
    float x, y, z;      // 位置
    float vx, vy, vz;   // 速度
    float mass;         // 质量
    int id;             // ID
};  // 32 bytes

struct ParticleAoS particles_aos[1000];

// 访问模式 (只更新位置)
for (int i = 0; i < 1000; i++) {
    particles_aos[i].x += particles_aos[i].vx * dt;
    particles_aos[i].y += particles_aos[i].vy * dt;
    particles_aos[i].z += particles_aos[i].vz * dt;
    // 加载32字节，只使用12字节，浪费50%缓存带宽！
}

// SoA - Structure of Arrays (数据导向设计)
struct ParticleSoA {
    float x[1000];
    float y[1000];
    float z[1000];
    float vx[1000];
    float vy[1000];
    float vz[1000];
    float mass[1000];
    int id[1000];
};

struct ParticleSoA particles_soa;

// 访问模式
for (int i = 0; i < 1000; i++) {
    particles_soa.x[i] += particles_soa.vx[i] * dt;
    particles_soa.y[i] += particles_soa.vy[i] * dt;
    particles_soa.z[i] += particles_soa.vz[i] * dt;
    // 连续访问x数组，缓存利用率100%！
}
