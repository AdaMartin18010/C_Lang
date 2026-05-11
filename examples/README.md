# C_Lang 代码示例库

本目录包含 14 个主题的 **可编译 C 代码示例**，覆盖 C 语言核心技术与工业实践。

## 目录索引

| 目录 | 主题 | 示例数 | 关键技术 |
|------|------|--------|----------|
| `bit_manipulation/` | 位操作 | 3 | 位图分配器、标志位、字节序 |
| `concurrency/` | 并发编程 | 3 | C11 threads、条件变量、原子操作 |
| `data_structures/` | 数据结构 | 5 | 侵入式链表、哈希表、动态数组、BST |
| `design_patterns/` | 设计模式 | 3 | 虚表多态、状态机、观察者模式 |
| `embedded/` | 嵌入式 | 3 | MMIO、位域寄存器、中断向量表 |
| `error_handling/` | 错误处理 | 4 | 错误码系统、goto cleanup、RAII、setjmp |
| `library_design/` | 库设计 | 3 | 不透明指针、API版本、插件系统 |
| `macros/` | 宏与元编程 | 3 | X-Macro、_Generic、字符串化 |
| `network/` | 网络编程 | 2 | select/epoll TCP服务器、客户端 |
| `system/` | 系统编程 | 4 | mmap、信号、fork、epoll |
| `misra_compliance/` | MISRA C:2023 | 4 | 类型安全、内存安全、控制流、函数安全 |
| `arena_allocator/` | Arena 分配器 | 1 | 工业级内存池，bump-pointer 分配 |
| `kv_store/` | KV 存储引擎 | 1 | 开放寻址哈希表，持久化，自动扩容 |
| `http_parser/` | HTTP 协议解析器 | 1 | 状态机解析 + select HTTP 服务器 |
| `rt_scheduler/` | 实时任务调度器 | 1 | RMS/EDF 调度 + 可调度性分析 |
| `protocol_serializer/` | 协议序列化引擎 | 1 | VarInt + TLV 编码解码 |
| `cve_cases/` | CVE 审计案例 | 1 | 缓冲区溢出、UAF、整数溢出 |
| `performance/` | 性能优化案例 | 1 | 缓存行、分支预测、循环展开 |
| `lock_free/` | 无锁数据结构 | 1 | SPSC Ring Buffer + 原子计数器 |
| `assembler/` | 汇编 | - | 已有示例 |
| `c23_c2y/` | C23/C2y | - | 已有示例 |
| `interpreter/` | 解释器 | - | 已有示例 |
| `lambda/` | Lambda | - | 已有示例 |
| `turing_machine/` | 图灵机 | - | 已有示例 |
| `vm/` | 虚拟机 | - | 已有示例 |

## 快速开始

```bash
cd <任意子目录>
make
./<程序名>
```

## 编译要求

- **GCC/Clang** 支持 C11 (`-std=c11`)
- **POSIX** 环境（Linux/macOS/WSL）用于系统/网络示例
- Windows 用户可用 MSYS2 或 WSL

## 设计原则

1. **零依赖**：所有示例仅用标准库 + POSIX
2. **独立可编译**：每个 `.c` 文件可单独编译运行
3. **自包含注释**：代码内包含编译命令和关键说明
