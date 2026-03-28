# OpenMP并行编程深度指南

> **层级定位**: 03_System_Technology_Domains / 16_High_Performance_Computing
> **难度级别**: L3-L4
> **对应标准**: OpenMP 4.5/5.0/5.1
> **最后更新**: 2026-03-28

---

## 📋 本节概要

| 属性 | 内容 |
|:-----|:-----|
| **核心概念** | OpenMP指令、并行循环、任务并行、同步机制、性能优化 |
| **前置知识** | [C语言基础](../../01_Core_Knowledge_System/)、[并发编程](../../14_Concurrency_Parallelism/) |
| **后续延伸** | [MPI编程](01_MPI_Programming.md)、[SIMD优化](03_SIMD_Optimization.md) |
| **横向关联** | [多线程](../../14_Concurrency_Parallelism/)、[性能调优](../../18_Systems_Performance/) |
| **权威来源** | OpenMP Specification, LLNL OpenMP Tutorial |

---

## 📑 目录

- [OpenMP并行编程深度指南](#openmp并行编程深度指南)
  - [📋 本节概要](#-本节概要)
  - [📑 目录](#-目录)
  - [🎯 OpenMP概述](#-openmp概述)
    - [什么是OpenMP？](#什么是openmp)
    - [OpenMP特性](#openmp特性)
  - [🔧 基本并行构造](#-基本并行构造)
    - [并行区域](#并行区域)
    - [并行循环](#并行循环)
  - [⚡ 同步机制](#-同步机制)
  - [📊 任务并行](#-任务并行)
  - [🔬 深入理解](#-深入理解)
    - [OpenMP运行时](#openmp运行时)
    - [性能优化](#性能优化)
    - [编译与运行](#编译与运行)
  - [✅ 质量验收清单](#-质量验收清单)
  - [📚 相关资源](#-相关资源)
    - [官方文档](#官方文档)
    - [编译器支持](#编译器支持)
    - [工具](#工具)

---

## 🎯 OpenMP概述

### 什么是OpenMP？

**OpenMP**（Open Multi-Processing）是一套用于共享内存并行编程的API，基于编译器指令（pragmas）。

```
并行编程模型对比：

OpenMP                    MPI
─────────────────         ─────────────────
共享内存                  分布式内存
├── 多线程模型            ├── 多进程模型
├── 隐式数据分布          ├── 显式消息传递
├── 适合单机多核          ├── 适合集群
└── 学习曲线平缓          └── 学习曲线陡峭

适用场景：
- 科学计算
- 图像处理
- 数值模拟
- 数据分析
```

### OpenMP特性

| 特性 | 说明 |
|:-----|:-----|
| **编译器指令** | `#pragma omp` 易于集成 |
| **自动并行** | 编译器处理线程管理 |
| **可扩展性** | 从2核到1000+核 |
| **可移植性** | 跨平台支持 |
| **嵌套并行** | 支持多级并行 |

---

## 🔧 基本并行构造

### 并行区域

```c
#include <omp.h>
#include <stdio.h>

// 基本并行区域
void basic_parallel() {
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int nthreads = omp_get_num_threads();
        printf("Hello from thread %d of %d\n", tid, nthreads);
    }
}

// 带选项的并行区域
void parallel_with_options() {
    // 指定线程数
    #pragma omp parallel num_threads(4)
    {
        // 4个线程执行此区域
    }

    // 条件并行
    #pragma omp parallel if(condition)
    {
        // 仅当condition为true时并行
    }

    // 私有变量
    int shared_var = 0;
    int private_var = 0;

    #pragma omp parallel private(private_var) shared(shared_var)
    {
        // private_var: 每个线程有自己的副本
        // shared_var: 所有线程共享
        private_var = omp_get_thread_num();
        #pragma omp atomic
        shared_var += private_var;
    }
}
```

### 并行循环

```c
#include <omp.h>

// 基本并行for循环
void parallel_for() {
    int n = 1000;
    double a[n], b[n], c[n];

    // 初始化
    for (int i = 0; i < n; i++) {
        a[i] = b[i] = i * 1.0;
    }

    // 并行计算 c = a + b
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

// 带约简的并行循环
void parallel_reduction() {
    int n = 1000000;
    double sum = 0.0;
    double product = 1.0;
    int max_val = 0;

    // 求和约简
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < n; i++) {
        sum += i;
    }

    // 乘法约简
    #pragma omp parallel for reduction(*:product)
    for (int i = 1; i <= 10; i++) {
        product *= i;
    }

    // 最大值约简
    #pragma omp parallel for reduction(max:max_val)
    for (int i = 0; i < n; i++) {
        int val = compute_value(i);
        if (val > max_val) max_val = val;
    }
}

// 调度策略
void scheduling_strategies() {
    int n = 1000;

    // static: 循环在编译时分配，均匀分布
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < n; i++) {
        // 每个线程获得n/num_threads个迭代
    }

    // static with chunk: 按块分配
    #pragma omp parallel for schedule(static, 64)
    for (int i = 0; i < n; i++) {
        // 每64个迭代为一组分配
    }

    // dynamic: 运行时动态分配
    #pragma omp parallel for schedule(dynamic, 32)
    for (int i = 0; i < n; i++) {
        // 每个线程完成32个迭代后请求更多
    }

    // guided: 动态分配，块大小逐渐减小
    #pragma omp parallel for schedule(guided)
    for (int i = 0; i < n; i++) {
        // 初始大块，逐渐减小
    }

    // auto: 编译器/运行时决定
    #pragma omp parallel for schedule(auto)
    for (int i = 0; i < n; i++) {
    }
}

// 循环折叠（OpenMP 4.5+）
void collapse_loops() {
    double a[100][100];

    // 并行化嵌套循环
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            a[i][j] = i * j;
        }
    }
}
```

---

## ⚡ 同步机制

```c
#include <omp.h>

// 屏障同步
void barrier_example() {
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();

        // 阶段1工作
        do_work_phase1(tid);

        // 等待所有线程到达此处
        #pragma omp barrier

        // 阶段2工作（确保所有线程完成阶段1）
        do_work_phase2(tid);
    }
}

// 临界区
void critical_section() {
    int counter = 0;

    #pragma omp parallel for
    for (int i = 0; i < 1000; i++) {
        #pragma omp critical
        {
            // 同一时刻只有一个线程执行
            counter++;
            shared_resource[i] = compute(i);
        }
    }

    // 命名临界区（允许多个独立的临界区）
    #pragma omp parallel
    {
        #pragma omp critical(resource_a)
        { access_resource_a(); }

        #pragma omp critical(resource_b)
        { access_resource_b(); }
    }
}

// 原子操作
void atomic_operations() {
    int counter = 0;
    double sum = 0.0;

    #pragma omp parallel for
    for (int i = 0; i < 1000; i++) {
        #pragma omp atomic
        counter++;

        #pragma omp atomic
        sum += compute(i);
    }
}

// 锁机制
void lock_mechanism() {
    omp_lock_t my_lock;
    omp_init_lock(&my_lock);

    #pragma omp parallel
    {
        // 获取锁
        omp_set_lock(&my_lock);

        // 临界区
        access_shared_resource();

        // 释放锁
        omp_unset_lock(&my_lock);
    }

    omp_destroy_lock(&my_lock);
}

// 主线程执行
void master_directive() {
    #pragma omp parallel
    {
        #pragma omp master
        {
            // 仅主线程（thread 0）执行
            printf("Master thread initializing...\n");
            initialize_global_data();
        }

        // 隐式屏障等待主线程完成

        // 所有线程继续执行
        #pragma omp for
        for (int i = 0; i < n; i++) {
            process(i);
        }
    }
}

// 单线程执行（第一个到达的线程）
void single_directive() {
    #pragma omp parallel
    {
        #pragma omp single
        {
            // 只有一个线程执行（不一定是主线程）
            read_input_file();
        }

        // 所有线程等待single完成

        #pragma omp for
        for (int i = 0; i < n; i++) {
            process(data[i]);
        }
    }
}
```

---

## 📊 任务并行

```c
#include <omp.h>

// 基本任务
void basic_tasks() {
    #pragma omp parallel
    {
        #pragma omp single
        {
            // 生成任务
            #pragma omp task
            { task_a(); }

            #pragma omp task
            { task_b(); }

            #pragma omp task
            { task_c(); }

            // 等待所有任务完成
            #pragma omp taskwait
        }
    }
}

// 递归任务（斐波那契）
int fib(int n) {
    if (n < 2) return n;

    int x, y;
    #pragma omp task shared(x)
    x = fib(n - 1);

    #pragma omp task shared(y)
    y = fib(n - 2);

    #pragma omp taskwait
    return x + y;
}

// 任务依赖
void task_dependencies() {
    #pragma omp parallel
    {
        #pragma omp single
        {
            int a, b, c;

            // 任务A
            #pragma omp task depend(out: a)
            { a = compute_a(); }

            // 任务B，依赖A
            #pragma omp task depend(in: a) depend(out: b)
            { b = compute_b(a); }

            // 任务C，依赖A
            #pragma omp task depend(in: a) depend(out: c)
            { c = compute_c(a); }

            // 任务D，依赖B和C
            #pragma omp task depend(in: b, c)
            { compute_d(b, c); }
        }
    }
}

// 任务循环
void taskloop() {
    // OpenMP 5.0+ 特性
    #pragma omp parallel
    {
        #pragma omp single
        {
            #pragma omp taskloop grainsize(100)
            for (int i = 0; i < 10000; i++) {
                // 每次迭代作为一个任务
                process_item(i);
            }
        }
    }
}
```

---

## 🔬 深入理解

### OpenMP运行时

```
OpenMP程序执行流程：

1. 串行区域（主线程）
      ↓
2. 遇到parallel指令
      ↓
3. 创建线程池（或重用已有）
      ↓
4. 并行区域执行
   ├── 工作分配（循环/任务）
   ├── 同步（屏障/锁）
   └── 线程私有数据管理
      ↓
5. 并行区域结束
   └── 隐式屏障（除非nowait）
      ↓
6. 串行区域继续
```

### 性能优化

```c
// 1. 减少同步开销
void reduce_synchronization() {
    // ❌ 频繁同步
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        #pragma omp critical
        { sum += a[i]; }  // 每个迭代都同步！
    }

    // ✅ 使用约简
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < n; i++) {
        sum += a[i];
    }

    // ✅ 或局部累加
    #pragma omp parallel
    {
        double local_sum = 0;
        #pragma omp for
        for (int i = 0; i < n; i++) {
            local_sum += a[i];
        }
        #pragma omp atomic
        sum += local_sum;
    }
}

// 2. 避免伪共享
void avoid_false_sharing() {
    // ❌ 可能导致伪共享
    int counters[8];  // 可能在一个缓存行
    #pragma omp parallel for
    for (int i = 0; i < 8; i++) {
        counters[i]++;
    }

    // ✅ 填充避免伪共享
    struct PaddedCounter {
        int value;
        char padding[60];  // 填充到64字节
    } counters[8];
}

// 3. 负载均衡
void load_balancing() {
    // 对于不均匀工作负载使用dynamic调度
    #pragma omp parallel for schedule(dynamic, 1)
    for (int i = 0; i < n; i++) {
        // 不同迭代工作量不同
        heavy_computation(i);
    }
}
```

### 编译与运行

```bash
# GCC编译OpenMP程序
gcc -fopenmp -O3 program.c -o program

# 设置线程数
export OMP_NUM_THREADS=8

# 运行时配置
export OMP_SCHEDULE=dynamic,64
export OMP_PROC_BIND=spread
export OMP_PLACES=cores

# 执行
./program
```

---

## ✅ 质量验收清单

- [x] OpenMP概述与特性
- [x] 并行区域（parallel）
- [x] 并行循环（parallel for）
- [x] 调度策略（schedule）
- [x] 约简操作（reduction）
- [x] 同步机制（barrier, critical, atomic, locks）
- [x] 任务并行（task, taskwait, depend）
- [x] 性能优化技巧
- [x] 编译运行指南

---

## 📚 相关资源

### 官方文档

- **OpenMP Specification**: <https://www.openmp.org/specifications/>
- **LLNL OpenMP Tutorial**: <https://computing.llnl.gov/tutorials/openMP/>

### 编译器支持

- GCC: `-fopenmp`
- Clang: `-fopenmp`
- MSVC: `/openmp`
- Intel: `-qopenmp`

### 工具

- **OMP Scout**: 性能分析工具
- **Intel VTune**: OpenMP性能分析

---

> **最后更新**: 2026-03-28
> **版本**: 1.0 - 完整版
> **维护者**: C_Lang Knowledge Base Team
