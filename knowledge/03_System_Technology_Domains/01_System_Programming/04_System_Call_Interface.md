# 系统调用接口 (System Call Interface)

> **层级定位**: 03 System Technology Domains / 01 System Programming
> **难度级别**: L4 熟练
> **预估学习时间**: 8-12 小时

## 概述

系统调用接口是用户空间程序与操作系统内核交互的桥梁。理解系统调用的机制、分类和实现原理，是深入掌握系统编程的基础。

## 核心概念

### 1. 系统调用机制
- **陷阱指令**: `int 0x80` (x86), `syscall`/`sysenter` (x86-64), `svc` (ARM)
- **系统调用表**: 内核维护的函数指针数组
- **参数传递**: 寄存器 vs 栈传递约定

### 2. 系统调用分类
| 类别 | 典型调用 | 说明 |
|:-----|:---------|:-----|
| 进程控制 | `fork`, `execve`, `exit`, `wait` | 进程创建、执行、终止 |
| 文件操作 | `open`, `read`, `write`, `close` | 文件描述符管理 |
| 设备管理 | `ioctl`, `mmap` | 设备控制和内存映射 |
| 信息维护 | `getpid`, `gettimeofday` | 系统信息查询 |
| 通信 | `pipe`, `socket`, `shmget` | IPC 机制 |

### 3. 用户空间封装
- **glibc 封装**: `syscall()` 和高级 API 的区别
- **Linux vDSO**: 虚拟动态共享对象加速常用调用
- **musl 实现**: 轻量级 C 库的系统调用方式

## 关键实现细节

### x86-64 Linux 调用约定
```c
// 系统调用号通过 rax 传递
// 参数: rdi, rsi, rdx, r10, r8, r9
// 返回值: rax (负数表示错误)

syscall(SYS_write, fd, buf, count);
```

### 错误处理
- `errno` 全局变量的线程安全性
- `strerror()` 和 `perror()` 的使用

## 与其他模块的关联

- [内存管理](../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md)
- [进程调度](../../16_OS_Kernel/02_Process_Scheduling_Deep.md)
- [eBPF 深度探索](06_eBPF_Deep.md)

## 待补充内容

- [ ] 各架构系统调用 ABI 详细对比 (x86, x86-64, ARM64, RISC-V)
- [ ] `strace` 使用和原理
- [ ] 系统调用开销分析和优化策略
- [ ] seccomp 和系统调用过滤

---

*最后更新: 2026-05-10*
