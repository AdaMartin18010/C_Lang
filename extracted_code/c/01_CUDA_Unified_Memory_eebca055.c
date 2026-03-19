/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\11_Heterogeneous_Memory\01_CUDA_Unified_Memory.md
 * Line: 142
 * Language: c
 * Block ID: eebca055
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 统一内存的页面迁移是按需进行的
// 由GPU页错误触发

/*
 * 页面迁移流程：
 * 1. GPU首次访问某页面 → GPU页错误
 * 2. 页面从CPU内存迁移到GPU内存
 * 3. 建立GPU页表映射
 * 4. 后续访问直接命中
 *
 * 反之亦然（GPU到CPU）
 */

// 页面迁移粒度：通常4KB或64KB（取决于架构）
#define PAGE_SIZE_4KB  (4 * 1024)
#define PAGE_SIZE_64KB (64 * 1024)

// 查看页面迁移统计
void check_migration_stats(void) {
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, 0);

    printf("设备: %s\n", prop.name);
    printf("统一内存支持: %s\n",
           prop.managedMemory ? "是" : "否");
    printf("并发管理访问: %s\n",
           prop.concurrentManagedAccess ? "是" : "否");

    // 在Linux上可以通过nvidia-smi查看迁移统计
    // nvidia-smi nvlink -e
    // nvidia-smi pmon -s u  # 显示统一内存迁移
}

// 理解访问模式对迁移的影响
__global__ void stride_kernel(float *data, int stride) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int strided_idx = idx * stride;

    // 如果stride大，每次访问不同页面
    // 会导致大量页面迁移
    data[strided_idx] = data[strided_idx] * 2.0f;
}

// 优化版本：合并访问
__global__ void coalesced_kernel(float *data) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    // 线程连续访问，同一warp访问同一/相邻页面
    data[idx] = data[idx] * 2.0f;
}
