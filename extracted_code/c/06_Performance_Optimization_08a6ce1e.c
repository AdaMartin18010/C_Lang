/*
 * Auto-generated from: 12_Practice_Exercises\06_Performance_Optimization.md
 * Line: 345
 * Language: c
 * Block ID: 08a6ce1e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Array of Structs (AoS) - 面向对象风格
typedef struct {
    float x, y, z;
    float vx, vy, vz;
    float mass;
} Particle;
Particle particles[1000];

// Struct of Arrays (SoA) - 数据导向设计
typedef struct {
    float x[1000], y[1000], z[1000];
    float vx[1000], vy[1000], vz[1000];
    float mass[1000];
} ParticlesSoA;

// SoA更适合SIMD处理
void update_positions_soa(ParticlesSoA *p, int n) {
    for (int i = 0; i < n; i += 8) {
        __m256 vx = _mm256_load_ps(&p->vx[i]);
        __m256 x = _mm256_load_ps(&p->x[i]);
        x = _mm256_add_ps(x, vx);
        _mm256_store_ps(&p->x[i], x);
        // ...
    }
}
