# 系统编程 (System Programming)

> **层级定位**: 03 System Technology Domains / 01 System Programming
> **难度级别**: L4 熟练
> **预估学习时间**: 40+ 小时

## 概述

系统编程是编写直接与操作系统硬件交互的软件的艺术，涉及进程管理、内存管理、文件系统、网络通信和并发控制等核心领域。与应用程序编程不同，系统编程要求深入理解底层机制和资源约束。

## 核心领域

### 1. 进程与线程
- 进程管理
- 线程与并发
- 进程间通信 (IPC)

### 2. 系统调用与内核接口
- [系统调用接口](04_System_Call_Interface.md)
- 内存映射文件
- [eBPF 深度探索](06_eBPF_Deep.md)

### 3. 内存管理
- [内存分配器设计](07_Memory_Allocator_Design.md)
- 垃圾回收
- jemalloc/tcmalloc 实现

### 4. 调试与分析
- 静态分析原理
- 动态分析原理
- 调试原理与框架
- 测试方法论

### 5. 性能工程
- [性能剖析原理](13_Performance_Profiling_Principles.md)
- [安全关键系统编程](14_Safety_Critical_Systems_Programming.md)

## 关键技术栈

| 领域 | 核心技术 | 工具 |
|:-----|:---------|:-----|
| 系统调用 | POSIX API, Linux-specific | `strace`, `syscall` |
| 并发 | pthreads, futex, io_uring | `perf`, `eBPF` |
| 内存 | `mmap`, `brk`, `mlock` | `valgrind`, `asan` |
| 网络 | `socket`, `epoll`, `io_uring` | `tcpdump`, `wireshark` |

## 学习路径

1. **基础**: C 语言核心 + 操作系统原理
2. **进阶**: 系统调用 + 并发编程
3. **深入**: 内核模块 + eBPF + 性能优化
4. **专家**: 自定义运行时 + 安全关键系统

## 待补充内容

- [ ] 各子模块详细文档
- [ ] 实战项目案例
- [ ] 性能基准测试数据

---

*最后更新: 2026-05-10*
