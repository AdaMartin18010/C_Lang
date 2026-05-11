# C_Lang 学习路径图（L1-L6）

## 总体架构

```
L6 ┌─────────────────────────────────────────────────────────┐
   │  领域专家（内核开发/安全关键/高性能系统）                  │
   │  • Linux 内核模块 / eBPF / 驱动开发                      │
   │  • MISRA C / DO-178C / ISO 26262 合规                   │
   │  • DPDK / RDMA / 零拷贝网络                              │
L5 ├─────────────────────────────────────────────────────────┤
   │  系统架构师                                              │
   │  • 内存分配器 / 并发框架 / 协议栈实现                      │
   │  • 数据库引擎 / 分布式共识 / 虚拟机                        │
   │  • 性能分析与极致优化                                    │
L4 ├─────────────────────────────────────────────────────────┤
   │  高级工程师                                              │
   │  • 系统编程（mmap/epoll/signal/fork）                    │
   │  • 网络编程（socket/select/epoll/io_uring）              │
   │  • 嵌入式（MMIO/中断/RTOS）                              │
L3 ├─────────────────────────────────────────────────────────┤
   │  熟练开发者                                              │
   │  • 库设计（ABI/API版本/不透明指针）                       │
   │  • 数据结构（侵入式链表/BST/哈希表）                      │
   │  • 设计模式（虚表/状态机/观察者）                         │
L2 ├─────────────────────────────────────────────────────────┤
   │  进阶程序员                                              │
   │  • 位操作 / 宏与元编程 / 错误处理模式                     │
   │  • C11 并发 / 原子操作 / 内存模型                         │
   │  • 编译与链接 / Makefile / CMake                          │
L1 ├─────────────────────────────────────────────────────────┤
   │  入门程序员                                              │
   │  • C 语法基础 / 数据类型 / 指针与数组                     │
   │  • 控制流 / 函数 / 结构体与联合体                         │
   │  • 标准库 / 文件 I/O / 字符串处理                         │
└─────────────────────────────────────────────────────────────┘
```

## 阶段详解

### L1 基础层（2-4周）

**目标**：能独立编写 200 行以内的程序

| 主题 | 核心文档 | 配套代码 |
|------|---------|---------|
| 语法基础 | `01_Basic_Layer/` | - |
| 指针与数组 | `02_Core_Layer/00_Array_Pointer_Deep_Dive.md` | - |
| 内存管理 | `02_Core_Layer/02_Memory_Management.md` | - |
| 字符串处理 | `02_Core_Layer/03_String_Processing.md` | - |
| 标准库 | `04_Standard_Library_Layer/` | - |

**里程碑**：实现一个命令行计算器 + 简单文件处理工具

### L2 进阶层（3-5周）

**目标**：掌握 C 语言特有的底层技术

| 主题 | 核心文档 | 配套代码 |
|------|---------|---------|
| 位操作 | `18_Bit_Manipulation/` | `examples/bit_manipulation/` |
| 错误处理 | `21_Error_Handling/` | `examples/error_handling/` |
| 宏与预处理器 | `20_Macros_Preprocessor/` | `examples/macros/` |
| C11 并发 | `14_Concurrency_Parallelism/` | `examples/concurrency/` |
| 构建系统 | `05_Engineering_Layer/` | - |

**里程碑**：实现一个带错误处理的配置文件解析器

### L3 熟练层（4-6周）

**目标**：能设计模块化、可重用的 C 组件

| 主题 | 核心文档 | 配套代码 |
|------|---------|---------|
| 数据结构 | `16_Linked_Data_Structures/` | `examples/data_structures/` |
| 设计模式 | `13_Design_Patterns/` | `examples/design_patterns/` |
| 库设计 | `28_Library_Design/` | `examples/library_design/` |
| 数值计算 | `17_Numerical_Computing/` | - |
| 可移植性 | `26_Portability/` | `examples/bit_manipulation/endianness.c` |

**里程碑**：实现一个通用的哈希表库（完整 API + 测试）

### L4 系统层（6-8周）

**目标**：掌握系统编程核心能力

| 主题 | 核心文档 | 配套代码 |
|------|---------|---------|
| 系统编程 | `38_System_Programming_Practical/` | `examples/system/` |
| 网络编程 | `37_Network_Programming_Practical/` | `examples/network/` |
| 嵌入式 | `36_Embedded_C_Practical/` | `examples/embedded/` |
| 调试与性能 | `06_Debugging_Performance/` | - |

**里程碑**：实现一个单线程 select Echo 服务器

### L5 架构层（持续）

**目标**：能设计和实现复杂系统组件

| 主题 | 核心文档 | 配套代码 |
|------|---------|---------|
| 内存分配器 | `03_System_Technology_Domains/01_System_Programming/07_Memory_Allocator_Design.md` | `examples/arena_allocator/` |
| 并发框架 | `07_Modern_Toolchain/07_Concurrency_Parallelism/` | `examples/thread_pool/` |
| 协议解析 | `03_System_Technology_Domains/15_Network_Programming/` | `examples/http_parser/` |
| KV 存储 | `03_System_Technology_Domains/11_In_Memory_Database/` | `examples/kv_store/` |

**里程碑**：实现一个带持久化的内存 KV 存储引擎

### L6 专家层（持续）

**目标**：在特定领域达到专业深度

| 方向 | 核心内容 |
|------|---------|
| 安全关键 | MISRA C / DO-178C / 形式化验证 |
| 操作系统 | Linux 内核 / eBPF / 内存子系统 |
| 网络系统 | DPDK / RDMA / 高性能协议栈 |
| 嵌入式 | RTOS / 安全启动 / 硬件安全 |
| 工业实践 | Redis / SQLite / Nginx 源码研读 |

**里程碑**：为开源项目贡献代码 / 通过安全认证审计

## 主题依赖图

```
                    ┌─────────────┐
                    │   L1 基础    │
                    │ 语法/指针/数组│
                    └──────┬──────┘
                           │
           ┌───────────────┼───────────────┐
           ▼               ▼               ▼
      ┌─────────┐    ┌─────────┐    ┌─────────┐
      │ 位操作   │    │ 错误处理 │    │ 标准库   │
      └────┬────┘    └────┬────┘    └────┬────┘
           │               │               │
           └───────────────┼───────────────┘
                           ▼
                    ┌─────────────┐
                    │   L2 进阶    │
                    │ 宏/并发/构建  │
                    └──────┬──────┘
                           │
           ┌───────────────┼───────────────┐
           ▼               ▼               ▼
      ┌─────────┐    ┌─────────┐    ┌─────────┐
      │数据结构  │    │设计模式  │    │库设计   │
      └────┬────┘    └────┬────┘    └────┬────┘
           │               │               │
           └───────────────┼───────────────┘
                           ▼
                    ┌─────────────┐
                    │   L3 熟练    │
                    │ 模块化设计   │
                    └──────┬──────┘
                           │
           ┌───────────────┼───────────────┐
           ▼               ▼               ▼
      ┌─────────┐    ┌─────────┐    ┌─────────┐
      │系统编程  │    │网络编程  │    │嵌入式   │
      └────┬────┘    └────┬────┘    └────┬────┘
           │               │               │
           └───────────────┼───────────────┘
                           ▼
                    ┌─────────────┐
                    │   L4 系统    │
                    │ POSIX/网络   │
                    └──────┬──────┘
                           │
           ┌───────────────┼───────────────┐
           ▼               ▼               ▼
      ┌─────────┐    ┌─────────┐    ┌─────────┐
      │内存分配器│    │并发框架  │    │KV存储   │
      └────┬────┘    └────┬────┘    └────┬────┘
           │               │               │
           └───────────────┼───────────────┘
                           ▼
                    ┌─────────────┐
                    │   L5 架构    │
                    │ 复杂系统设计 │
                    └──────┬──────┘
                           │
           ┌───────────────┼───────────────┐
           ▼               ▼               ▼
      ┌─────────┐    ┌─────────┐    ┌─────────┐
      │内核开发  │    │安全关键  │    │网络系统  │
      └─────────┘    └─────────┘    └─────────┘
                           │
                           ▼
                    ┌─────────────┐
                    │   L6 专家    │
                    │ 领域深度     │
                    └─────────────┘
```

## 交叉引用索引

| 能力 | 涉及文档 | 涉及代码示例 |
|------|---------|------------|
| 安全编码 | `09_Safety_Standards/` | `examples/misra_compliance/` |
| 并发编程 | `14_Concurrency_Parallelism/`, `07_Concurrency_Parallelism/` | `examples/concurrency/` |
| 网络服务 | `37_Network_Programming_Practical/`, `15_Network_Programming/` | `examples/network/`, `examples/system/` |
| 嵌入式 | `36_Embedded_C_Practical/`, `08_Application_Domains/02_Embedded_Systems.md` | `examples/embedded/` |
| 数据结构 | `16_Linked_Data_Structures/`, `02_Core_Layer/10_Advanced_Data_Structures.md` | `examples/data_structures/` |
| 库设计 | `28_Library_Design/` | `examples/library_design/` |
| 系统编程 | `38_System_Programming_Practical/`, `01_System_Programming/` | `examples/system/` |
| 调试优化 | `06_Debugging_Performance/`, `05_Engineering_Layer/` | - |

---

> **更新**: 2026-05-11
> **覆盖**: 1,199 文档 / 979K 行 / 50+ 可编译代码示例
