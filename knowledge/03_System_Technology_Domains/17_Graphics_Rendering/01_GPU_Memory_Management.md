# GPU内存管理

> **目标**: 详解CUDA/OpenCL中的GPU内存管理与优化
>
> **技术栈**: CUDA, OpenCL, Vulkan Compute
>
> **难度**: L5 | **预估时间**: 6-8小时

---

## 目录

- [GPU内存管理](#gpu内存管理)
  - [目录](#目录)
  - [1. GPU内存层次](#1-gpu内存层次)
    - [1.1 GPU内存架构](#11-gpu内存架构)
  - [2. CUDA内存类型](#2-cuda内存类型)
    - [2.1 主机与设备内存](#21-主机与设备内存)
  - [3. 内存分配策略](#3-内存分配策略)
    - [3.1 内存池实现](#31-内存池实现)
  - [4. 数据传输优化](#4-数据传输优化)
    - [4.1 异步传输](#41-异步传输)
  - [5. 统一内存](#5-统一内存)
  - [6. 性能调优](#6-性能调优)
    - [6.1 内存访问模式](#61-内存访问模式)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## 1. GPU内存层次

### 1.1 GPU内存架构

```
GPU内存层次 (NVIDIA Ampere架构)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

┌─────────────────────────────────────────────────────────────┐
│  寄存器 (Register)                                          │
│  • 每个线程私有                                              │
│  • 延迟: 1周期                                               │
│  • 容量: 64K (A100) / 128K (H100) 每SM                      │
│  • 最快，但有限                                              │
├─────────────────────────────────────────────────────────────┤
│  共享内存 / L1缓存 (Shared Memory / L1)                      │
│  • 同一块内线程共享                                          │
│  • 延迟: 20-30周期                                           │
│  • 容量: 164KB/SM (可配置)                                   │
│  • 可编程缓存                                                │
├─────────────────────────────────────────────────────────────┤
│  L2缓存                                                      │
│  • 所有SM共享                                                │
│  • 延迟: 200周期                                             │
│  • 容量: 40MB (A100) / 50MB (H100)                          │
│  • 自动管理                                                  │
├─────────────────────────────────────────────────────────────┤
│  全局内存 (Global Memory) = 显存                            │
│  • 所有线程可访问                                            │
│  • 延迟: 400-800周期                                         │
│  • 容量: 40GB-80GB (HBM2e/HBM3)                             │
│  • 通过PCIe与CPU内存交换                                     │
└─────────────────────────────────────────────────────────────┘
```

---

## 2. CUDA内存类型

### 2.1 主机与设备内存

```cuda
// CUDA内存分配模式
#include <cuda_runtime.h>

// 1. 可分页内存 (Pageable)
void* host_pageable = malloc(size);
cudaMemcpy(device_ptr, host_pageable, size, cudaMemcpyHostToDevice);

// 2. 页锁定内存 (Pinned/Locked)
void* host_pinned;
cudaMallocHost(&host_pinned, size);
cudaMemcpy(device_ptr, host_pinned, size, cudaMemcpyHostToDevice);
cudaFreeHost(host_pinned);

// 3. 设备内存 (Device)
void* device_ptr;
cudaMalloc(&device_ptr, size);
cudaFree(device_ptr);
```

---

## 3. 内存分配策略

### 3.1 内存池实现

```cuda
typedef struct {
    void** free_list;
    size_t free_count;
    size_t block_size;
} CudaMemoryPool;

void* cuda_pool_alloc(CudaMemoryPool* pool) {
    if (pool->free_count > 0) {
        return pool->free_list[--pool->free_count];
    }
    void* ptr;
    cudaMalloc(&ptr, pool->block_size);
    return ptr;
}
```

---

## 4. 数据传输优化

### 4.1 异步传输

```cuda
void pipelined_processing(float* host_data, size_t total_size) {
    cudaStream_t streams[NUM_STREAMS];
    float* device_buffers[NUM_STREAMS];

    for (int i = 0; i < num_chunks; i++) {
        int stream_id = i % NUM_STREAMS;

        // 异步H2D拷贝
        cudaMemcpyAsync(device_buffers[stream_id],
                       host_data + i * chunk_size,
                       chunk_size, cudaMemcpyHostToDevice,
                       streams[stream_id]);

        // 内核执行 (与拷贝重叠)
        process_kernel<<<grid, block, 0, streams[stream_id]>>>(
            device_buffers[stream_id]
        );
    }
}
```

---

## 5. 统一内存

```cuda
// CUDA统一内存
void* managed_ptr;
cudaMallocManaged(&managed_ptr, size);

// 预取优化
cudaMemPrefetchAsync(managed_ptr, size, device_id, stream);

// 建议只读
cudaMemAdvise(managed_ptr, size,
              cudaMemAdviseSetReadMostly, device_id);
```

---

## 6. 性能调优

### 6.1 内存访问模式

```cuda
// BAD: 非合并访问
__global__ void bad_access(float* data) {
    int idx = threadIdx.x;
    float val = data[idx * 1000];  // 步长大
}

// GOOD: 合并访问
__global__ void good_access(float* data) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    float val = data[idx];  // 连续线程访问连续地址
}
```

---

> **核心洞察**: GPU内存管理的关键在于最大化数据局部性、使用异步操作避免空闲、以及确保合并内存访问模式。

---

*最后更新: 2026-03-14 | CUDA 12.x*


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
