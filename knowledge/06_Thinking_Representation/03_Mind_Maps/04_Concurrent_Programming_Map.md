---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
﻿# 并发编程思维导图

## 概述

本文档使用思维导图形式系统梳理并发编程的核心概念、线程管理、同步机制、常见问题及性能优化策略。

---

## 思维导图结构

```
                           ┌─────────────────┐
                           │  并发编程总览    │
                           │  Concurrent     │
                           │  Programming    │
                           └────────┬────────┘
                                    │
        ┌───────────┬───────────────┼───────────────┬───────────┐
        ▼           ▼               ▼               ▼           ▼
   ┌─────────┐ ┌─────────┐   ┌─────────────┐  ┌───────────┐ ┌───────────┐
   │并发基础 │ │线程管理 │   │  同步机制   │  │ 并发问题  │ │ 性能优化  │
   │ Basics  │ │ Thread  │   │ Synchronization│ │  Issues   │ │Optimization│
   │         │ │ Mgmt    │   │             │  │           │ │           │
   └────┬────┘ └────┬────┘   └──────┬──────┘  └─────┬─────┘ └─────┬─────┘
        │           │               │               │             │
        ▼           ▼               ▼               ▼             ▼
```

---

## 一、并发基础 (Concurrency Basics)

- **核心概念**
  - **并发 (Concurrency)**：多个任务在重叠时间段内执行
  - **并行 (Parallelism)**：多个任务真正同时执行（多核）
  - **进程 (Process)**：独立地址空间，资源分配的基本单位
  - **线程 (Thread)**：共享进程地址空间，CPU 调度的基本单位

- **并发模型**
  - 多进程模型
  - 多线程模型
  - 协程/纤程模型 (Coroutine/Fiber)
  - 事件驱动模型 (Event-driven)
  - Actor 模型

- **线程状态**
  - 新建 (New)
  - 就绪 (Ready)
  - 运行 (Running)
  - 阻塞 (Blocked/Waiting)
  - 终止 (Terminated)

- **上下文切换**
  - 保存当前线程状态
  - 加载下一个线程状态
  - 切换开销：寄存器、程序计数器、栈指针

---

## 二、线程管理分支 (Thread Management)

### 2.1 线程创建

- **POSIX 线程 (pthreads)**
  - `pthread_create()` - 创建线程
  - `pthread_exit()` - 线程退出
  - `pthread_join()` - 等待线程结束

- **C11 线程**
  - `thrd_create()` - 创建线程
  - `thrd_join()` - 等待线程

- **线程池模式**
  - 预创建固定数量线程
  - 任务队列分发
  - 减少创建/销毁开销

### 2.2 线程终止

- 正常返回
- 调用线程退出函数
- 被其他线程取消 (pthread_cancel)
- 进程退出导致所有线程终止

### 2.3 线程属性

- 分离状态 (Detached)
- 栈大小
- 调度策略
- 优先级

### 2.4 线程局部存储 (TLS)

- `thread_local` 关键字 (C11)
- `pthread_key_create()`
- 每个线程独立的数据副本

---

## 三、同步机制分支 (Synchronization Mechanisms)

### 3.1 互斥锁 (Mutex)

- **基本概念**
  - 互斥访问临界区
  - 二元状态：锁定/解锁

- **操作**
  - `pthread_mutex_init()` - 初始化
  - `pthread_mutex_lock()` - 加锁
  - `pthread_mutex_trylock()` - 尝试加锁（非阻塞）
  - `pthread_mutex_unlock()` - 解锁
  - `pthread_mutex_destroy()` - 销毁

- **类型**
  - 普通互斥锁
  - 递归互斥锁（同线程可重复加锁）
  - 自适应互斥锁

- **最佳实践**
  - 最小化临界区
  - 避免在持有锁时进行 I/O
  - 使用 RAII 封装（自动解锁）

### 3.2 信号量 (Semaphore)

- **概念**
  - 计数器，控制多个资源的访问
  - P 操作（wait/decrement）
  - V 操作（signal/increment）

- **类型**
  - 二进制信号量（0/1，类似互斥锁）
  - 计数信号量（资源池管理）

- **POSIX 信号量**
  - 有名信号量（进程间）
  - 无名信号量（线程间）
  - `sem_init()`, `sem_wait()`, `sem_post()`

### 3.3 条件变量 (Condition Variable)

- **用途**
  - 线程间等待特定条件
  - 与互斥锁配合使用

- **操作**
  - `pthread_cond_init()` - 初始化
  - `pthread_cond_wait()` - 等待（自动释放锁）
  - `pthread_cond_signal()` - 唤醒一个等待线程
  - `pthread_cond_broadcast()` - 唤醒所有等待线程
  - `pthread_cond_destroy()` - 销毁

- **典型模式 - 生产者/消费者**

  ```c
  pthread_mutex_lock(&mutex);
  while (condition_not_met) {
      pthread_cond_wait(&cond, &mutex);
  }
  // 执行操作
  pthread_mutex_unlock(&mutex);
  ```

### 3.4 读写锁 (Read-Write Lock)

- **特点**
  - 多读单写
  - 读共享，写独占

- **适用场景**
  - 读多写少的场景
  - 提高并发读取性能

### 3.5 屏障 (Barrier)

- **功能**
  - 等待指定数量线程到达
  - 然后同时放行

- **应用**
  - 并行算法阶段同步
  - `pthread_barrier_init()`, `pthread_barrier_wait()`

### 3.6 原子操作 (Atomic Operations)

- `stdatomic.h` (C11)
- 无锁编程基础
- 原子类型：`atomic_int`, `atomic_flag`
- 内存序：`memory_order_relaxed`, `memory_order_seq_cst`

---

## 四、并发问题分支 (Concurrency Issues)

### 4.1 竞态条件 (Race Condition)

- **定义**
  - 多个线程访问共享数据
  - 执行顺序影响结果
  - 结果不可预测

- **示例**

  ```c
  // 非原子操作
  counter++;  // 读取-修改-写入，可能丢失更新
  ```

- **解决**
  - 互斥锁保护
  - 原子操作
  - 线程局部存储

### 4.2 死锁 (Deadlock)

- **定义**
  - 多个线程互相等待对方释放资源
  - 永久阻塞

- **产生条件（Coffman 条件）**
  - 互斥条件
  - 占有并等待
  - 不可抢占
  - 循环等待

- **预防策略**
  - 锁顺序一致（全局排序）
  - 尝试获取锁（超时机制）
  - 一次性获取所有锁
  - 使用层次锁

- **检测与恢复**
  - 超时检测
  - 死锁检测算法
  - 线程终止/资源抢占

### 4.3 活锁 (Livelock)

- **定义**
  - 线程不断改变状态响应彼此
  - 实际没有进展

- **区别**
  - 死锁：完全阻塞
  - 活锁：持续运行但无结果

- **解决**
  - 引入随机等待
  - 退避策略（指数退避）

### 4.4 饥饿 (Starvation)

- **定义**
  - 某些线程长期得不到执行机会

- **原因**
  - 优先级不公平
  - 锁竞争激烈

- **解决**
  - 公平锁（FIFO）
  - 优先级调整

### 4.5 其他问题

- **ABA 问题**：CAS 操作中的值变化又恢复
- **优先级反转**：低优先级线程持有高优先级线程需要的锁
- **伪共享 (False Sharing)**：缓存行竞争

---

## 五、性能优化分支 (Performance Optimization)

### 5.1 锁优化

- **细粒度锁**
  - 减少临界区范围
  - 按数据结构/段加锁

- **锁分离**
  - 读写分离
  - 哈希桶独立锁

- **无锁结构**
  - 原子操作代替锁
  - 无锁队列、栈
  - RCU (Read-Copy-Update)

- **减少锁竞争**
  - 线程局部存储
  - 每个线程独立处理，最后合并

### 5.2 缓存优化

- **避免伪共享**
  - 填充缓存行（Padding）
  - 分离热点数据

- **数据局部性**
  - 连续内存访问
  - 预取友好结构

### 5.3 线程池优化

- 合理设置线程数
  - CPU 密集型：N 或 N+1
  - IO 密集型：2N 或更多

- 工作窃取 (Work Stealing)
  - 空闲线程从繁忙线程获取任务
  - 负载均衡

### 5.4 异步与并行算法

- 任务分解
- 流水线并行
- SIMD 向量化
- 并行排序、归约

### 5.5 工具与技巧

- **性能分析**
  - Profiler 识别热点
  - 锁竞争分析

- **调试工具**
  - ThreadSanitizer (TSan)
  - Helgrind
  - Lockdep

---

## 六、快速参考卡片

| 机制 | 适用场景 | 关键函数 |
|------|----------|----------|
| 互斥锁 | 保护临界区 | `pthread_mutex_lock/unlock` |
| 条件变量 | 等待条件 | `pthread_cond_wait/signal` |
| 信号量 | 资源计数 | `sem_wait/post` |
| 读写锁 | 读多写少 | `pthread_rwlock_rdlock/wrlock` |
| 屏障 | 阶段同步 | `pthread_barrier_wait` |
| 原子操作 | 简单计数 | `atomic_fetch_add` |

---

## 七、并发设计原则

1. **最小共享**：减少共享数据
2. **不变性**：优先使用不可变数据
3. **线程安全**：接口设计考虑并发
4. **清晰所有权**：明确谁负责同步
5. **优先高级抽象**：使用成熟的并发库

---

> **状态**: ✅ 已完成


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
