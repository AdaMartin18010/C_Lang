/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\07_Microarchitecture\README.md
 * Line: 616
 * Language: c
 * Block ID: 2bf94e79
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 数组结构体 (SoA) vs 结构体数组 (AoS)

// AoS: 缓存不友好, 但局部性好
struct ParticleAoS {
    float x, y, z;      // 位置
    float vx, vy, vz;   // 速度
    float mass;         // 质量
};  // 28 字节

// SoA: SIMD 友好, 缓存高效
struct ParticleSoA {
    float* x; float* y; float* z;
    float* vx; float* vy; float* vz;
    float* mass;
};

// 使用场景选择
void update_positions_aos(ParticleAoS* particles, int n) {
    // 适合: 随机访问单个粒子所有属性
    for (int i = 0; i < n; i++) {
        particles[i].x += particles[i].vx;
        particles[i].y += particles[i].vy;
        particles[i].z += particles[i].vz;
    }
}

void update_positions_soa(ParticleSoA* p, int n) {
    // 适合: SIMD 向量化, 顺序访问
    int i = 0;
    for (; i + 7 < n; i += 8) {
        __m256 vx = _mm256_loadu_ps(&p->vx[i]);
        __m256 vy = _mm256_loadu_ps(&p->vy[i]);
        __m256 vz = _mm256_loadu_ps(&p->vz[i]);

        __m256 x = _mm256_loadu_ps(&p->x[i]);
        __m256 y = _mm256_loadu_ps(&p->y[i]);
        __m256 z = _mm256_loadu_ps(&p->z[i]);

        _mm256_storeu_ps(&p->x[i], _mm256_add_ps(x, vx));
        _mm256_storeu_ps(&p->y[i], _mm256_add_ps(y, vy));
        _mm256_storeu_ps(&p->z[i], _mm256_add_ps(z, vz));
    }
}
