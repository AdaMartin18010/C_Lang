# C_Lang 核心术语英中对照词汇表

> **范围**: C 语言、系统编程、嵌入式、安全标准领域核心术语
> **条目**: ~200 条

---

## A

| English | 中文 | 备注 |
|---------|------|------|
| ABI (Application Binary Interface) | 应用二进制接口 | 调用约定、数据布局、寄存器使用 |
| Alignment | 对齐 | 内存地址对齐要求 |
| Arena Allocator | 区域分配器 | 同生命周期对象批量分配/释放 |
| Arithmetic Conversion | 算术转换 | 整数提升 + 通常算术转换 |
| Array Decay | 数组退化 | 数组在表达式中退化为指针 |
| Atomic Operation | 原子操作 | 不可中断的读-改-写 |

## B

| English | 中文 | 备注 |
|---------|------|------|
| Bit Field | 位域 | struct 中指定宽度的字段 |
| Bit Manipulation | 位操作 | 与/或/非/异或/移位 |
| Buffer Overflow | 缓冲区溢出 | 写入超出分配边界 |
| Build System | 构建系统 | Makefile/CMake/Meson |
| Bump Pointer | 碰撞指针 | Arena 分配器中的分配指针 |

## C

| English | 中文 | 备注 |
|---------|------|------|
| Cache Line | 缓存行 | CPU 缓存最小传输单位（通常 64B） |
| Calling Convention | 调用约定 | 参数传递和栈清理规则 |
| Cast | 类型转换 | 显式类型转换 |
| Ceiling | 上取整 | 调度算法中的利用率上限 |
| Circular Buffer | 环形缓冲区 | 首尾相接的 FIFO 队列 |
| Compliance | 合规 | 符合标准/规范 |
| Compound Literal | 复合字面量 | C99 `(Type){init}` |
| Concurrency | 并发 | 多任务同时推进 |
| Const Correctness | const 正确性 | 正确使用 const 限定 |
| Context Switch | 上下文切换 | 保存/恢复进程状态 |
| Critical Section | 临界区 | 访问共享数据的代码段 |

## D

| English | 中文 | 备注 |
|---------|------|------|
| Data Race | 数据竞争 | 无同步的并发访问 |
| Deadlock | 死锁 | 循环等待资源 |
| Declaration | 声明 | 引入标识符及其类型 |
| Definition | 定义 | 声明 + 存储分配/实现 |
| Dereference | 解引用 | 通过指针访问目标 |
| Designated Initializer | 指定初始化器 | C99 `.field = value` |
| DMA (Direct Memory Access) | 直接内存访问 | 外设直接读写内存 |
| DRY (Don't Repeat Yourself) | 不重复自己 | 消除重复代码 |

## E

| English | 中文 | 备注 |
|---------|------|------|
| EDF (Earliest Deadline First) | 最早截止期优先 | 动态优先级实时调度 |
| Embedded System | 嵌入式系统 | 专用计算机系统 |
| Endianness | 字节序 | 大端/小端 |
| Escape Sequence | 转义序列 | `\n`, `\t`, `\xFF` 等 |

## F

| English | 中文 | 备注 |
|---------|------|------|
| False Sharing | 伪共享 | 不同 CPU 修改同一缓存行 |
| File Descriptor | 文件描述符 | POSIX 中标识打开文件的整数 |
| FIFO | 先进先出 | 队列顺序 |
| Fixed-Width Integer | 固定宽度整数 | `int32_t`, `uint64_t` |
| Forward Declaration | 前向声明 | 不完整的类型声明 |
| Function Pointer | 函数指针 | 指向函数的指针 |

## G

| English | 中文 | 备注 |
|---------|------|------|
| Generic Selection | 泛型选择 | C11 `_Generic` |
| goto Cleanup | goto 清理 | 统一资源释放模式 |

## H

| English | 中文 | 备注 |
|---------|------|------|
| Header File | 头文件 | `.h` 接口声明 |
| HeartBleed | 心脏出血 | CVE-2014-0160 漏洞 |

## I

| English | 中文 | 备注 |
|---------|------|------|
| IEC 61508 | 功能安全标准 | 工业安全 |
| Incomplete Type | 不完整类型 | 如 `struct X;`（未定义成员） |
| Inline Function | 内联函数 | `static inline` |
| Integer Promotion | 整数提升 | 小整数类型转为 `int` |
| Interrupt Service Routine | 中断服务程序 | ISR |
| Intrusive Data Structure | 侵入式数据结构 | 节点包含链表指针 |
| ISO 26262 | 道路车辆功能安全 | 汽车安全标准 |

## K

| English | 中文 | 备注 |
|---------|------|------|
| Kernel | 内核 | 操作系统核心 |
| Key-Value Store | 键值存储 | KV 存储 |

## L

| English | 中文 | 备注 |
|---------|------|------|
| L-value | 左值 | 可出现在赋值左侧的表达式 |
| Linker | 链接器 | 解析符号引用 |
| Lock-Free | 无锁 | 不依赖互斥锁的并发算法 |
| LRU (Least Recently Used) | 最近最少使用 | 缓存淘汰策略 |
| LSB (Least Significant Bit) | 最低有效位 | |
| LTO (Link Time Optimization) | 链接时优化 | 跨模块优化 |

## M

| English | 中文 | 备注 |
|---------|------|------|
| Macro | 宏 | 预处理器文本替换 |
| Memory Barrier | 内存屏障 | 防止指令重排 |
| Memory Leak | 内存泄漏 | 分配后未释放 |
| Memory Order | 内存序 | `memory_order_seq_cst` 等 |
| MISRA C | MISRA C | 汽车行业 C 语言编码规范 |
| MMIO (Memory-Mapped I/O) | 内存映射 I/O | 通过内存地址访问外设 |
| Mutex | 互斥锁 | 互斥访问共享资源 |

## N

| English | 中文 | 备注 |
|---------|------|------|
| Name Mangling | 名称修饰 | C++ 编译器修改函数名 |
| NaN (Not a Number) | 非数字 | IEEE 754 特殊值 |
| Non-blocking I/O | 非阻塞 I/O | 不会等待的 I/O 操作 |

## O

| English | 中文 | 备注 |
|---------|------|------|
| Object File | 目标文件 | `.o` / `.obj` |
| Opaque Pointer | 不透明指针 | 隐藏实现细节的指针 |
| Optimization | 优化 | 编译器或手工性能改进 |
| Overflow | 溢出 | 超出类型表示范围 |
| Ownership | 所有权 | 资源管理责任归属 |

## P

| English | 中文 | 备注 |
|---------|------|------|
| Parallelism | 并行 | 多任务同时执行 |
| Parameter | 形参 | 函数定义中的参数 |
| Parsing | 解析 | 语法分析 |
| Peripheral | 外设 | 处理器外部的硬件 |
| Pointer Arithmetic | 指针运算 | `p + 1`, `p - q` |
| POSIX | 可移植操作系统接口 | IEEE 标准 API |
| Preprocessor | 预处理器 | `#include`, `#define` |

## R

| English | 中文 | 备注 |
|---------|------|------|
| Race Condition | 竞态条件 | 结果依赖于执行时序 |
| RAII | 资源获取即初始化 | C++ 概念，C 中模拟 |
| Real-Time | 实时 | 确定性时间约束 |
| Recursion | 递归 | 函数调用自身 |
| RMS (Rate Monotonic Scheduling) | 速率单调调度 | 固定优先级实时调度 |
| RTOS | 实时操作系统 | FreeRTOS, Zephyr 等 |

## S

| English | 中文 | 备注 |
|---------|------|------|
| Sanitizer | 消毒器 | AddressSanitizer, UBSan |
| Scope | 作用域 | 标识符可见范围 |
| Segmentation Fault | 段错误 | 非法内存访问 |
| Sequence Point | 序列点 | 副作用完成点 |
| Side Effect | 副作用 | 表达式除返回值外的效果 |
| Signal | 信号 | POSIX 异步通知机制 |
| SIMD | 单指令多数据 | SSE/AVX/NEON |
| SPSC (Single Producer Single Consumer) | 单生产者单消费者 | 一种无锁队列模型 |
| Stack Overflow | 栈溢出 | 栈空间耗尽 |
| Static Analysis | 静态分析 | 不运行代码的检查 |
| Strict Aliasing | 严格别名 | 通过不同类型的指针访问同一内存 |
| Struct Padding | 结构体填充 | 编译器插入的对齐字节 |
| Stub | 存根 | 未实现的占位代码 |

## T

| English | 中文 | 备注 |
|---------|------|------|
| Tag | 标签 | `struct`, `union`, `enum` 的名称 |
| TLV (Tag-Length-Value) | 标签-长度-值 | 二进制编码格式 |
| Tombstone | 墓碑 | 标记已删除但保留探测链的条目 |
| Translation Unit | 翻译单元 | 一个 `.c` 文件及其包含的头 |
| Type Punning | 类型双关 | 通过不同类型访问同一数据 |
| Type Qualifier | 类型限定符 | `const`, `volatile`, `restrict` |

## U

| English | 中文 | 备注 |
|---------|------|------|
| UAF (Use-After-Free) | 释放后使用 | 访问已释放的内存 |
| UB (Undefined Behavior) | 未定义行为 | C 标准未规定的行为 |
| uintptr_t | 无符号整数足够容纳指针 | `<stdint.h>` |
| Union | 联合体 | 共享内存的不同类型视图 |
| Usual Arithmetic Conversions | 通常算术转换 | 二元运算前的类型统一 |

## V

| English | 中文 | 备注 |
|---------|------|------|
| VarInt | 变长整数 | 7 位编码的可变长度整数 |
| VLA (Variable Length Array) | 变长数组 | C99 特性，C11 可选 |
| VTable | 虚表 | C 中模拟多态的函数指针表 |
| volatile | 易变的 | 防止编译器优化访问 |

## W

| English | 中文 | 备注 |
|---------|------|------|
| Watchdog | 看门狗 | 系统复位定时器 |
| WCET (Worst Case Execution Time) | 最坏执行时间 | 实时系统分析指标 |
| Weak Symbol | 弱符号 | 可被覆盖的符号定义 |

## X

| English | 中文 | 备注 |
|---------|------|------|
| X-Macro | X 宏 | 数据驱动宏元编程技术 |

---

## 缩写速查

| 缩写 | 全称 | 中文 |
|------|------|------|
| API | Application Programming Interface | 应用编程接口 |
| AST | Abstract Syntax Tree | 抽象语法树 |
| BSS | Block Started by Symbol | 未初始化数据段 |
| CI/CD | Continuous Integration / Deployment | 持续集成/部署 |
| COFF | Common Object File Format | 通用目标文件格式 |
| CRC | Cyclic Redundancy Check | 循环冗余校验 |
| CS | Critical Section | 临界区 |
| ELF | Executable and Linkable Format | 可执行与可链接格式 |
| EP | Entry Point | 入口点 |
| FP | Function Pointer / Frame Pointer | 函数指针 / 帧指针 |
| GP | Global Pointer | 全局指针 |
| IDB | Induced Database | 诱导数据库（符号表） |
| ILP | Instruction Level Parallelism | 指令级并行 |
| IPC | Inter-Process Communication | 进程间通信 |
| IR | Intermediate Representation | 中间表示 |
| ISR | Interrupt Service Routine | 中断服务例程 |
| PC | Program Counter | 程序计数器 |
| PE | Portable Executable | 可移植可执行文件（Windows） |
| PIC | Position Independent Code | 位置无关代码 |
| PLT | Procedure Linkage Table | 过程链接表 |
| RE | Reverse Engineering | 逆向工程 |
| RISC | Reduced Instruction Set Computer | 精简指令集计算机 |
| RTTI | Run-Time Type Information | 运行时类型信息 |
| SB | Stack Buffer | 栈缓冲区 |
| SBOM | Software Bill of Materials | 软件物料清单 |
| SF | Stack Frame | 栈帧 |
| SP | Stack Pointer | 栈指针 |
| SPL | Software Product Line | 软件产品线 |
| SRA | Structure Reliability Analysis | 结构可靠性分析 |
| SSP | Stack Smashing Protector | 栈溢出保护 |
| ST | Software Testing | 软件测试 |
| SW | Software | 软件 |
| TCB | Trusted Computing Base | 可信计算基 |
| TDD | Test-Driven Development | 测试驱动开发 |
| TLS | Thread-Local Storage | 线程本地存储 |
| TOC | Table of Contents | 目录表 |
| TLB | Translation Lookaside Buffer | 转译后备缓冲区 |
| TRM | Technical Reference Manual | 技术参考手册 |
| UART | Universal Asynchronous Receiver/Transmitter | 通用异步收发器 |
| VM | Virtual Machine / Virtual Memory | 虚拟机 / 虚拟内存 |
| VPN | Virtual Private Network | 虚拟专用网络 |
| WAR | Write-After-Read | 读后写（数据冒险） |
| WAW | Write-After-Write | 写后写（数据冒险） |
| WP | Write Protect | 写保护 |
| XIP | Execute In Place | 原地执行 |

---

> **更新**: 2026-05-11
> **标准**: ISO/IEC 9899:2024, MISRA C:2023, CERT C
