---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/readme.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/readme.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/readme.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/readme.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/readme.md) | 核心关联 | 思维表征基础 |
﻿# GPU显存管理

> **层级定位**: 04 Industrial Scenarios / 05 Game Engine
> **对应标准**: CUDA Runtime API, OpenCL, Vulkan Memory Model
> **难度级别**: L5
> **预估学习时间**: 8-10小时

---

## 📋 本节概要

| 属性 | 内容 |
|:-----|:-----|
| **核心概念** | GPU内存层次、显存分配、数据传输、统一内存 |
| **前置知识** | C指针、内存管理、并行编程基础 |
| **后续延伸** | CUDA优化、图形渲染、深度学习推理 |
| **权威来源** | NVIDIA CUDA C Programming Guide, Vulkan Spec |

---


---

## 📑 目录

- [GPU显存管理](#gpu显存管理)
  - [📋 本节概要](#-本节概要)
  - [📑 目录](#-目录)
  - [📖 1. GPU内存架构](#-1-gpu内存架构)
    - [1.1 内存层次结构](#11-内存层次结构)
    - [1.2 CUDA内存类型](#12-cuda内存类型)
  - [📖 2. 显存分配与释放](#-2-显存分配与释放)
    - [2.1 CUDA运行时API](#21-cuda运行时api)
    - [2.2 统一内存 (Unified Memory)](#22-统一内存-unified-memory)
    - [2.3 内存池与异步分配](#23-内存池与异步分配)
  - [📖 3. 内存传输优化](#-3-内存传输优化)
    - [3.1 页锁定内存 (Pinned Memory)](#31-页锁定内存-pinned-memory)
    - [3.2 异步数据传输与计算重叠](#32-异步数据传输与计算重叠)
  - [📖 4. 共享内存优化](#-4-共享内存优化)
  - [📖 5. 内存访问模式](#-5-内存访问模式)
    - [5.1 合并访问 vs 非合并访问](#51-合并访问-vs-非合并访问)
  - [⚠️ 常见陷阱](#️-常见陷阱)
    - [陷阱 GPU01: 未检查CUDA错误](#陷阱-gpu01-未检查cuda错误)
    - [陷阱 GPU02: 主机设备指针混淆](#陷阱-gpu02-主机设备指针混淆)
    - [陷阱 GPU03: 内存泄漏](#陷阱-gpu03-内存泄漏)
  - [✅ 质量验收清单](#-质量验收清单)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 📖 1. GPU内存架构

### 1.1 内存层次结构

```text
┌─────────────────────────────────────────────────────────────┐
│                    Host (CPU) Memory                        │
│                    (System RAM ~16-128GB)                   │
└──────────────────────┬──────────────────────────────────────┘
                       │ PCIe Bus (~16-32 GB/s)
                       ↓
┌─────────────────────────────────────────────────────────────┐
│                   Device (GPU) Memory                       │
├─────────────┬─────────────┬─────────────┬───────────────────┤
│  Global     │   Shared    │   Local     │    Constant       │
│  (~40GB)    │  (~164KB)   │  (per thread│    (~64KB)       │
│             │   per SM    │   spilled)  │                   │
└─────────────┴─────────────┴─────────────┴───────────────────┘
```

### 1.2 CUDA内存类型

| 内存类型 | 作用域 | 生命周期 | 访问速度 | 容量 |
|:---------|:-------|:---------|:---------|:-----|
| Global | Grid + Host | 应用程序 | 慢 (~400GB/s) | 大 |
| Shared | Block | Kernel | 快 (~10TB/s) | 小 (48-164KB/SM) |
| Local | Thread | Kernel | 中等 | 寄存器溢出 |
| Constant | Grid + Host | 应用程序 | 快 (缓存) | 64KB |
| Texture | Grid + Host | 应用程序 | 快 (缓存) | 大 |
| Registers | Thread | Kernel | 最快 | 255个/thread |

---

## 📖 2. 显存分配与释放

### 2.1 CUDA运行时API

```c
#include <cuda_runtime.h>
#include <stdio.h>
#include <stdlib.h>

// 显存分配示例
void cuda_memory_basics() {
    const int N = 1024 * 1024;  // 1M elements
    size_t size = N * sizeof(float);

    // 主机内存分配
    float *h_data = (float*)malloc(size);

    // 设备显存分配
    float *d_data;
    cudaError_t err = cudaMalloc((void**)&d_data, size);
    if (err != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed: %s\n", cudaGetErrorString(err));
        return;
    }

    // 初始化主机数据
    for (int i = 0; i < N; i++) {
        h_data[i] = (float)i;
    }

    // 主机到设备传输 (H2D)
    err = cudaMemcpy(d_data, h_data, size, cudaMemcpyHostToDevice);
    if (err != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy H2D failed: %s\n", cudaGetErrorString(err));
    }

    // ... 在GPU上执行核函数 ...

    // 设备到主机传输 (D2H)
    err = cudaMemcpy(h_data, d_data, size, cudaMemcpyDeviceToHost);
    if (err != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy D2H failed: %s\n", cudaGetErrorString(err));
    }

    // 释放资源
    free(h_data);
    cudaFree(d_data);
}
```

### 2.2 统一内存 (Unified Memory)

```c
#include <cuda_runtime.h>

// 统一内存 - 自动迁移
void unified_memory_example() {
    const int N = 1024 * 1024;
    size_t size = N * sizeof(float);

    float *data;

    // 分配统一内存
    cudaMallocManaged(&data, size);

    // 在CPU上初始化
    for (int i = 0; i < N; i++) {
        data[i] = (float)i;
    }

    // 预取到GPU (可选优化)
    int device = 0;
    cudaMemPrefetchAsync(data, size, device, NULL);

    // 启动核函数 (自动在GPU上执行)
    // kernel<<<blocks, threads>>>(data, N);

    // 同步确保完成
    cudaDeviceSynchronize();

    // 现在在CPU上访问数据 (自动迁移回主机)
    float sum = 0;
    for (int i = 0; i < N; i++) {
        sum += data[i];
    }

    cudaFree(data);
}
```

### 2.3 内存池与异步分配

```c
#include <cuda_runtime.h>

// CUDA内存池 (CUDA 11.2+)
void memory_pool_example() {
    cudaMemPool_t memPool;
    cudaDeviceGetMemPool(&memPool, 0);

    // 设置内存池属性
    uint64_t threshold = UINT64_MAX;  // 不释放回OS
    cudaMemPoolSetAttribute(memPool,
        cudaMemPoolAttrReleaseThreshold, &threshold);

    float *d_data;
    // 异步分配
    cudaMallocAsync(&d_data, 1024 * sizeof(float), 0);

    // 使用...

    // 异步释放 (可重用)
    cudaFreeAsync(d_data, 0);
}
```

---

## 📖 3. 内存传输优化

### 3.1 页锁定内存 (Pinned Memory)

```c
#include <cuda_runtime.h>

void pinned_memory_example() {
    const int N = 1024 * 1024;
    size_t size = N * sizeof(float);

    float *h_pinned;
    float *d_data;

    // 分配页锁定主机内存 (更快传输)
    cudaMallocHost(&h_pinned, size);  // 或 cudaHostAlloc
    cudaMalloc(&d_data, size);

    // 初始化
    for (int i = 0; i < N; i++) {
        h_pinned[i] = (float)i;
    }

    // 页锁定内存传输更快 (~12GB/s vs ~3GB/s for pageable)
    cudaMemcpy(d_data, h_pinned, size, cudaMemcpyHostToDevice);

    // 释放
    cudaFreeHost(h_pinned);
    cudaFree(d_data);
}
```

### 3.2 异步数据传输与计算重叠

```c
#include <cuda_runtime.h>

// 流与异步传输
void async_transfer_example() {
    const int N = 1024 * 1024;
    const int chunkSize = N / 4;  // 分成4个块
    size_t chunkBytes = chunkSize * sizeof(float);

    cudaStream_t streams[4];
    float *h_data, *d_data[4];

    // 分配页锁定内存
    cudaMallocHost(&h_data, N * sizeof(float));
    cudaMalloc(&d_data[0], N * sizeof(float));

    // 创建流
    for (int i = 0; i < 4; i++) {
        cudaStreamCreate(&streams[i]);
    }

    // 异步分块传输和处理
    for (int i = 0; i < 4; i++) {
        float *h_chunk = h_data + i * chunkSize;
        float *d_chunk = d_data[0] + i * chunkSize;

        // 异步H2D传输
        cudaMemcpyAsync(d_chunk, h_chunk, chunkBytes,
                       cudaMemcpyHostToDevice, streams[i]);

        // 在流中启动核函数 (与传输重叠)
        // process_kernel<<<blocks, threads, 0, streams[i]>>>(d_chunk);
    }

    // 同步所有流
    cudaDeviceSynchronize();

    // 清理
    for (int i = 0; i < 4; i++) {
        cudaStreamDestroy(streams[i]);
    }
    cudaFreeHost(h_data);
    cudaFree(d_data[0]);
}
```

---

## 📖 4. 共享内存优化

```c
// 使用共享内存的矩阵乘法示例
__global__ void matrixMulShared(float *C, float *A, float *B, int width) {
    __shared__ float As[16][16];
    __shared__ float Bs[16][16];

    int bx = blockIdx.x, by = blockIdx.y;
    int tx = threadIdx.x, ty = threadIdx.y;

    int row = by * 16 + ty;
    int col = bx * 16 + tx;

    float Cvalue = 0;

    // 分块计算
    for (int m = 0; m < width / 16; m++) {
        // 协作加载A和B到共享内存
        As[ty][tx] = A[row * width + (m * 16 + tx)];
        Bs[ty][tx] = B[(m * 16 + ty) * width + col];

        __syncthreads();  // 等待所有线程加载完成

        // 计算部分结果
        for (int k = 0; k < 16; k++) {
            Cvalue += As[ty][k] * Bs[k][tx];
        }

        __syncthreads();  // 确保计算完成再加载下一块
    }

    C[row * width + col] = Cvalue;
}
```

---

## 📖 5. 内存访问模式

### 5.1 合并访问 vs 非合并访问

```c
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
```

---

## ⚠️ 常见陷阱

### 陷阱 GPU01: 未检查CUDA错误

```c
// 错误
float *d_data;
cudaMalloc(&d_data, size);  // 失败时无提示
cudaMemcpy(d_data, h_data, size, cudaMemcpyHostToDevice);

// 正确
#define cudaCheckError(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line) {
    if (code != cudaSuccess) {
        fprintf(stderr, "GPUassert: %s %s %d\n",
                cudaGetErrorString(code), file, line);
        exit(code);
    }
}

cudaCheckError(cudaMalloc(&d_data, size));
```

### 陷阱 GPU02: 主机设备指针混淆

```c
float *h_data = (float*)malloc(size);
float *d_data;
cudaMalloc(&d_data, size);

// 错误：在设备指针上使用主机函数
free(d_data);  // 崩溃！

// 正确
cudaFree(d_data);
free(h_data);
```

### 陷阱 GPU03: 内存泄漏

```c
void leaky_function() {
    float *d_temp;
    cudaMalloc(&d_temp, 1024 * sizeof(float));
    // 忘记 cudaFree(d_temp)!
}

// 使用RAII模式或确保配对
void clean_function() {
    float *d_temp;
    cudaMalloc(&d_temp, 1024 * sizeof(float));
    // ... 使用 ...
    cudaFree(d_temp);  // 确保释放
}
```

---

## ✅ 质量验收清单

- [x] GPU内存层次结构
- [x] CUDA显存分配API
- [x] 统一内存使用
- [x] 页锁定内存优化
- [x] 异步传输与流
- [x] 共享内存优化
- [x] 合并访问模式
- [x] 常见陷阱分析

---

> **更新记录**
>
> - 2025-03-09: 重写充实，从模板63行扩展至完整技术文档


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
