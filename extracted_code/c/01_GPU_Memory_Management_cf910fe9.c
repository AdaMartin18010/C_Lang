/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\01_GPU_Memory_Management.md
 * Line: 326
 * Language: c
 * Block ID: cf910fe9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 合并访问 - 连续线程访问连续地址 (推荐)
__global__ void coalescedAccess(float *data) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    // 线程0访问data[0], 线程1访问data[1], ...
    float val = data[idx];  // 合并访问
}

// 非合并访问 - 连续线程访问分散地址 (避免)
__global__ void uncoalescedAccess(float *data, int stride) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    // 线程访问间隔地址
    float val = data[idx * stride];  // 非合并访问
}

// 结构体数组(AoS) vs 数组结构体(SoA)
// AoS - 访问模式差
struct ParticleAoS {
    float x, y, z;  // 位置
    float vx, vy, vz;  // 速度
};

// SoA - 访问模式好 (推荐)
struct ParticleSoA {
    float *x, *y, *z;
    float *vx, *vy, *vz;
};
