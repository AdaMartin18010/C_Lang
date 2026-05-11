# C语言核心术语英中对照表

> **用途**: 为国际化做准备，统一核心概念的中英文表达  
> **范围**: C语言标准、形式语义、编译原理、系统编程  
> **状态**: 初版 | **最后更新**: 2026-05-11  

---

## A

| 英文 | 中文 | 说明 |
|:-----|:-----|:-----|
| Abstract State Machine | 抽象状态机 | C标准定义的程序执行模型 |
| Activation Record | 激活记录 | 函数调用时在栈上创建的数据结构 |
| Alignment | 对齐 | 对象地址必须满足的边界条件 |
| Arena Allocator | Arena分配器 | 预分配大块内存、批量释放的分配策略 |
| Array-to-Pointer Decay | 数组到指针的衰减 | 数组表达式在大多数上下文中转为指向首元素的指针 |
| as-if Rule | as-if规则 | 编译器优化必须保持可观测行为一致的规则 |
| Atomic Operation | 原子操作 | 不可被中断的操作 |

## B

| 英文 | 中文 | 说明 |
|:-----|:-----|:-----|
| Borrow (Pointer) | 借用（指针）| 临时访问但不拥有资源 |
| Buffer Overflow | 缓冲区溢出 | 写入超出缓冲区边界 |
| Built-in Type | 内置类型 | 语言直接支持的类型（int, char等）|

## C

| 英文 | 中文 | 说明 |
|:-----|:-----|:-----|
| Calling Convention | 调用约定 | 函数调用时参数传递和寄存器使用的规则 |
| Constant Expression | 常量表达式 | C23中可在编译时求值的表达式 |
| dangling pointer | 悬垂指针 | 指向已释放内存的指针 |
| Dynamic Linking | 动态链接 | 程序运行时加载共享库 |
| Effective Type | 有效类型 | 对象当前持有的值的类型 |
| ELF (Executable and Linkable Format) | ELF格式 | Linux/Unix的可执行文件格式 |
| Escape Sequence | 转义序列 | 如 \n, \t 等特殊字符表示 |
| Explicit Conversion | 显式转换 | (type)expr 强制类型转换 |
| External Linkage | 外部链接 | 标识符在不同翻译单元中指向同一对象 |

## F

| 英文 | 中文 | 说明 |
|:-----|:-----|:-----|
| Function Pointer | 函数指针 | 指向函数的指针 |
| Function Prototype | 函数原型 | 函数的声明（不含实现）|

## G

| 英文 | 中文 | 说明 |
|:-----|:-----|:-----|
| Global Offset Table (GOT) | 全局偏移表 | PIC中用于间接访问全局数据的表 |

## H

| 英文 | 中文 | 说明 |
|:-----|:-----|:-----|
| Header File | 头文件 | 包含声明的接口文件（.h）|

## I

| 英文 | 中文 | 说明 |
|:-----|:-----|:-----|
| Implementation-defined Behavior | 实现定义行为 | 标准允许多种选择，实现必须文档化 |
| Indeterminate Value | 不确定值 | 未初始化对象的值 |
| Integer Promotion | 整数提升 | 小整数类型在运算前转为int |

## L

| 英文 | 中文 | 说明 |
|:-----|:-----|:-----|
| Lifetime | 生命周期 | 对象从创建到销毁的时间段 |
| Linker | 链接器 | 将目标文件合并为可执行文件的工具 |
| Lvalue | 左值 | 可出现在赋值左侧的表达式 |

## M

| 英文 | 中文 | 说明 |
|:-----|:-----|:-----|
| Memory Model | 内存模型 | 多线程环境下内存访问的语义规则 |
| Memory Safety | 内存安全 | 防止越界访问、Use-after-free等错误 |
| MISRA C | MISRA C | 汽车电子安全编码标准 |
| Multi-threading | 多线程 | 程序中多个执行流的并发执行 |

## N

| 英文 | 中文 | 说明 |
|:-----|:-----|:-----|
| nullptr | 空指针常量 | C23引入的类型安全空指针 |

## O

| 英文 | 中文 | 说明 |
|:-----|:-----|:-----|
| Object | 对象 | 存储中的数据区域 |
| Object Representation | 对象表示 | 对象中存储的字节序列 |
| Observable Behavior | 可观测行为 | as-if规则下必须保持一致的行为 |
| Owned Pointer | 拥有指针 | 负责释放所指向资源的指针 |

## P

| 英文 | 中文 | 说明 |
|:-----|:-----|:-----|
| Padding | 填充 | 编译器在结构体成员间插入的空白字节 |
| Pointer Arithmetic | 指针运算 | 指针加减整数、指针相减等操作 |
| Position Independent Code (PIC) | 位置无关代码 | 可在任意地址加载的代码 |
| Procedure Linkage Table (PLT) | 过程链接表 | 动态链接中用于延迟绑定的跳转表 |

## R

| 英文 | 中文 | 说明 |
|:-----|:-----|:-----|
| Race Condition | 竞态条件 | 多个线程访问共享数据的结果依赖于执行顺序 |
| Relocation | 重定位 | 链接器修改代码中的地址引用 |
| Representation | 表示 | 对象在存储中的字节模式 |
| Rvalue | 右值 | 不能出现在赋值左侧的表达式 |

## S

| 英文 | 中文 | 说明 |
|:-----|:-----|:-----|
| Scope | 作用域 | 标识符可见的代码区域 |
| Sequence Point | 序列点 | 副作用必须完成的执行点 |
| Side Effect | 副作用 | 改变程序状态的操作 |
| Stack Frame | 栈帧 | 函数调用时在栈上分配的内存区域 |
| Static Analysis | 静态分析 | 不运行程序而进行的代码分析 |
| Storage Class | 存储类 | auto, static, extern, register等说明符 |
| Storage Duration | 存储期 | 对象占用存储的时间长度 |
| Strict Aliasing Rule | 严格别名规则 | 限制通过不同类型指针访问同一对象的规则 |
| Symbol Resolution | 符号解析 | 链接器将符号引用与定义关联 |

## T

| 英文 | 中文 | 说明 |
|:-----|:-----|:-----|
| Translation Unit | 翻译单元 | 单个源文件预处理后的结果 |
| Trap Representation | 陷阱表示 | 不表示类型任何合法值的位模式 |
| Type Punning | 类型双关 | 通过不同类型访问同一对象表示 |
| Type Qualifier | 类型限定符 | const, volatile, restrict等 |

## U

| 英文 | 中文 | 说明 |
|:-----|:-----|:-----|
| Undefined Behavior (UB) | 未定义行为 | 标准完全未约束的行为 |
| Unspecified Behavior | 未指定行为 | 标准允许多种选择但无需文档化的行为 |
| Use-after-free | 释放后使用 | 访问已释放内存的指针 |

## V

| 英文 | 中文 | 说明 |
|:-----|:-----|:-----|
| Value Computation | 值计算 | 确定表达式值的过程 |
| Variable Length Array (VLA) | 变长数组 | C99引入、C11可选、C23保留VM类型的数组 |
| Volatile | 易失的 | 告诉编译器该对象可能被外部修改 |

## W

| 英文 | 中文 | 说明 |
|:-----|:-----|:-----|
| Working Draft (WD) | 工作草案 | 标准制定过程中的中间版本 |

---

## 附录：标准组织与文档

| 英文缩写 | 全称 | 中文 |
|:---------|:-----|:-----|
| ISO/IEC | International Organization for Standardization / International Electrotechnical Commission | 国际标准化组织/国际电工委员会 |
| WG14 | ISO/IEC JTC1/SC22/WG14 | C语言标准工作组 |
| Nxxxx | WG14 Document Number | WG14文档编号 |
| MISRA | Motor Industry Software Reliability Association | 汽车工业软件可靠性协会 |
| CERT | Computer Emergency Response Team | 计算机应急响应组 |
| GSL | Guidelines Support Library | 指南支持库 |

---

*此词汇表将持续更新，目标覆盖C23标准全部术语。*
