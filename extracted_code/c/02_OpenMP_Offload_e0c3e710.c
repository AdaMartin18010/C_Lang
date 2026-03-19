/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\11_Heterogeneous_Memory\02_OpenMP_Offload.md
 * Line: 149
 * Language: c
 * Block ID: e0c3e710
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// OpenMP Offload的数据映射
// map子句控制主机和设备间的数据传输

// 隐式数据映射（根据使用模式推断）
void implicit_mapping(float *a, float *b, float *c, int N) {
    // a,b,c在target区域使用，自动映射到设备
    // 默认行为：
    // - 标量和数组：tofrom（进入时复制到设备，退出时复制回）
    // - 指针：仅指针值，不指向的数据
    #pragma omp target parallel for
    for (int i = 0; i < N; i++) {
        c[i] = a[i] + b[i];
    }
}

// 显式数据映射
void explicit_mapping(float *a, float *b, float *c, int N) {
    // map子句显式指定数据传输行为
    #pragma omp target parallel for \
        map(to: a[0:N], b[0:N]) \
        map(from: c[0:N])
    for (int i = 0; i < N; i++) {
        c[i] = a[i] + b[i];
    }
}

// 数据映射类型
void mapping_types(void) {
    float data[1000];
    float result[1000];

    // to: 只复制到设备（设备只读）
    #pragma omp target map(to: data[0:1000])
    {
        // 读取data，修改不会传回主机
    }

    // from: 只从设备复制回（设备只写）
    #pragma omp target map(from: result[0:1000])
    {
        // 写入result，初始值未定义
    }

    // tofrom: 双向复制（默认）
    #pragma omp target map(tofrom: data[0:1000])
    {
        // 读取并修改data，修改会传回
    }

    // alloc: 在设备上分配，不初始化
    #pragma omp target map(alloc: result[0:1000])
    {
        // result在设备上未初始化
        // 所有元素必须先写入再读取
    }
}

// 结构体映射
struct Particle {
    float x, y, z;
    float vx, vy, vz;
    float mass;
};

void struct_mapping(struct Particle *particles, int N) {
    // 结构体数组映射
    #pragma omp target parallel for \
        map(tofrom: particles[0:N])
    for (int i = 0; i < N; i++) {
        particles[i].x += particles[i].vx;
        particles[i].y += particles[i].vy;
        particles[i].z += particles[i].vz;
    }
}
