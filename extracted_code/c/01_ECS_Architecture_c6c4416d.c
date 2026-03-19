/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\01_ECS_Architecture.md
 * Line: 271
 * Language: c
 * Block ID: c6c4416d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 缓存行大小通常为64字节
#define CACHE_LINE_SIZE 64

// ==================== AoS (Array of Structs) ====================
// 内存布局: [Entity0:Pos+Vel+Health][Entity1:Pos+Vel+Health]...
// 问题：访问Position时，Velocity和Health也被加载到缓存，造成浪费

typedef struct {
    Position pos;      // 12 bytes
    Velocity vel;      // 12 bytes
    Health health;     // 4 bytes
    // 可能还有padding到64字节对齐
} EntityData;

void update_aos(EntityData* entities, int count) {
    for (int i = 0; i < count; i++) {
        // 加载64字节缓存行，但只使用12字节的Position
        entities[i].pos.x += entities[i].vel.vx;
        // 强制写回，因为缓存行被修改
    }
}

// ==================== SoA (Struct of Arrays) ====================
// 内存布局: [所有Pos.x][所有Pos.y][所有Pos.z][所有Vel.vx]...
// 优势：处理Position时，只加载Position数据，缓存利用率100%

typedef struct {
    float* x;          // 连续数组
    float* y;
    float* z;
} SoAPosition;

typedef struct {
    float* vx;
    float* vy;
    float* vz;
} SoAVelocity;

void update_soa(SoAPosition* pos, SoAVelocity* vel, int count) {
    // 预取和SIMD友好
    for (int i = 0; i < count; i++) {
        // 64字节缓存行包含16个float，处理16个实体才需要一次内存访问
        pos->x[i] += vel->vx[i];
    }
}

// ==================== 性能对比 ====================
// 假设：10000个实体，计算位置更新
//
// AoS方式:
//   - 每次迭代访问1个EntityData (~64字节)
//   - 10000次 × 64字节 = 640KB内存流量
//   - 缓存行利用率: 12/64 = 18.75%
//
// SoA方式:
//   - 只访问pos.x数组 (4字节/实体)
//   - 10000次 × 4字节 = 40KB内存流量
//   - 缓存行利用率: 100%
//   - 理论加速比: 640/40 = 16x (实际约8-10x)
