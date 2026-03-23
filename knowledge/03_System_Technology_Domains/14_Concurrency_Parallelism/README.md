# 并发与并行编程

> **层级定位**: 03_System_Technology_Domains / 14_Concurrency_Parallelism
> **难度级别**: L4 (高级) → L5 (专家)
> **前置知识**: [内存管理](../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md)、[指针](../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md)、[函数指针](../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md)

---

## 关联导航

### 前置知识 (必须掌握)

- [内存管理](../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) - 共享内存基础
- [指针与别名](../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) - 数据竞争的理解基础
- [C11内存模型](../01_C11_Memory_Model.md) - 原子操作和内存序

### 后续延伸

- [无锁编程](./05_Lock_Free.md) - 高级并发技术
- [线程池实现](./04_Thread_Pool_Implementation.md) - 实际应用
- [并发调试](./06_Debugging.md) - 问题诊断
- [性能优化](../../01_Core_Knowledge_System/05_Engineering/03_Performance_Optimization.md) - 多核优化

### 理论支撑

- [理论统一映射](../../06_Thinking_Representation/05_Concept_Mappings/10_Theory_Unification_Mapping.md) - 内存模型统一视角
- [全局不变式](../../06_Thinking_Representation/05_Concept_Mappings/13_Global_Invariants.md) - 并发不变式
- [定理依赖网络](../../06_Thinking_Representation/05_Concept_Mappings/12_Theorem_Dependency_Network.md) - 数据竞争自由定理

---

## 模块概述

本模块涵盖C语言并发编程的完整知识体系，从POSIX线程基础到无锁高级技术。

## 目录

- [C11线程基础](./01_C11_Threads.md)
- [同步原语](./02_Synchronization_Primitives.md)
- [原子操作与内存序](./03_Atomic_Operations.md)
- [线程池实现](./04_Thread_Pool_Implementation.md)
- [无锁编程](./05_Lock_Free.md)
- [并发调试](./06_Debugging.md)

---

**最后更新**: 2026-03-24
**维护者**: C_Lang Knowledge Base Team
