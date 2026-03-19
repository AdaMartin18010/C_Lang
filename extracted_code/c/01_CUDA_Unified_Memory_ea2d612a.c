/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\11_Heterogeneous_Memory\01_CUDA_Unified_Memory.md
 * Line: 487
 * Language: c
 * Block ID: ea2d612a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 结构体数组 vs 数组结构体
// 对统一内存访问模式有重要影响

// 较差：结构体数组（AoS）
// 如果只需要访问部分字段，会迁移不必要的数据
typedef struct {
    float x, y, z;      // 位置
    float vx, vy, vz;   // 速度
    float mass;         // 质量
    int id;             // ID
} Particle_AoS;

// 较好：数组结构体（SoA）
// 可以独立访问不同属性，减少不必要迁移
typedef struct {
    float *x, *y, *z;
    float *vx, *vy, *vz;
    float *mass;
    int *id;
} Particle_SoA;

// 使用SoA的N体模拟
void nbody_simulation_soa(int N, int steps) {
    Particle_SoA particles;

    // 分配统一内存
    cudaMallocManaged(&particles.x, N * sizeof(float));
    cudaMallocManaged(&particles.y, N * sizeof(float));
    cudaMallocManaged(&particles.z, N * sizeof(float));
    cudaMallocManaged(&particles.vx, N * sizeof(float));
    cudaMallocManaged(&particles.vy, N * sizeof(float));
    cudaMallocManaged(&particles.vz, N * sizeof(float));

    // 初始化...

    // 只预取位置数据用于力计算
    cudaMemPrefetchAsync(particles.x, N * sizeof(float), 0, 0);
    cudaMemPrefetchAsync(particles.y, N * sizeof(float), 0, 0);
    cudaMemPrefetchAsync(particles.z, N * sizeof(float), 0, 0);

    for (int s = 0; s < steps; s++) {
        // 计算力（只需要位置）
        compute_forces_soa<<<blocks, threads>>>(
            particles.x, particles.y, particles.z,
            // 其他参数...
        );

        // 更新速度（需要速度和力）
        update_velocity_soa<<<blocks, threads>>>(
            particles.vx, particles.vy, particles.vz,
            // 力数据...
        );

        // 更新位置
        update_position_soa<<<blocks, threads>>>(
            particles.x, particles.y, particles.z,
            particles.vx, particles.vy, particles.vz
        );
    }

    // 清理...
}
