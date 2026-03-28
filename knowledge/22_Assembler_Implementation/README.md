# 汇编器实现 (Assembler Implementation)

> **模块定位**: Month 2 - 从机器语义到数据/控制映射
> **核心使命**: 理解汇编语言如何桥接高级语言和机器指令
> **预计时间**: 2个月
> **模块状态**: 核心文档完成

---

## 模块概述

本模块深入理解汇编语言的本质：

- 它是人类可读的机器指令表示
- 它是编译器输出的最终形式
- 它是理解C语言底层实现的钥匙

### 为什么需要这一层？

在理解了CPU如何执行指令之后，我们需要理解：

1. 如何用文本描述机器指令（汇编语言）
2. 如何将汇编翻译为机器码（汇编器）
3. C语言的数据和控制结构如何映射到汇编

```
认知链条:
CPU硬件 (执行机器码)
    ↓
机器指令 (二进制)
    ↓
汇编语言 (人类可读)
    ↓
汇编器 (符号解析、指令编码)
    ↓
C语言数据/控制结构
```

---

## 文件结构

### 基础汇编器

| 文件 | 内容 | 状态 | 前置知识 |
|:-----|:-----|:----:|:---------|
| [01_Hack_Assembler.md](./01_Hack_Assembler.md) | Hack汇编器实现 | ✅ 完成 | 数字逻辑基础 |
| [02_RISCV_Assembler.md](./02_RISCV_Assembler.md) | RISC-V汇编器实现 | ✅ 完成 | 01_Hack_Assembler |

### C到汇编映射

| 文件 | 内容 | 状态 | 前置知识 |
|:-----|:-----|:----:|:---------|
| [03_C_to_Assembly_Data.md](./03_C_to_Assembly_Data.md) | C数据结构的汇编表示 | ✅ 完成 | 02_RISCV_Assembler, 指针深度 |
| [04_C_to_Assembly_Control.md](./04_C_to_Assembly_Control.md) | C控制流的汇编实现 | ✅ 完成 | 03_C_to_Assembly_Data |

---

## 学习路径

### 初学者路径

```
1. 阅读 01_Hack_Assembler.md
   └── 理解汇编器的基本原理（两遍扫描）
2. 实现 Hack汇编器
   └── 熟悉符号表、指令编码
3. 阅读 02_RISCV_Assembler.md
   └── 理解真实处理器的复杂性
4. 阅读 03_C_to_Assembly_Data.md
   └── 理解数据类型的底层表示
5. 阅读 04_C_to_Assembly_Control.md
   └── 理解控制流的底层实现
```

### 有经验者路径

```
如果你已有汇编经验:
1. 快速浏览 02_RISCV_Assembler.md
   └── 关注RISC-V指令格式和重定位
2. 重点阅读 03_C_to_Assembly_Data.md
   └── 深入理解C类型的内存布局
3. 重点阅读 04_C_to_Assembly_Control.md
   └── 深入理解函数调用和优化
```

---

## 关键连接点

| 本模块概念 | 连接文档 |
|:-----------|:---------|
| 指令编码 | → [RISC-V CPU实现](../02_Formal_Semantics_and_Physics/10_ISA_Machine_Code/08_RISCV_CPU_Complete/README.md) |
| 符号解析 | → [链接加载](../02_Formal_Semantics_and_Physics/08_Linking_Loading_Topology/README.md) |
| 调用约定 | → [C函数调用](../01_Core_Knowledge_System/02_Core_Layer/04_Functions_Scope.md) |
| 数据布局 | → [C指针深度](../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) |
| 控制流 | → [C控制流](../01_Core_Knowledge_System/01_Basic_Layer/04_Control_Flow.md) |

---

## 下一步

完成本模块后，继续学习：

- [虚拟机实现](../23_VM_Implementation/) - 中间抽象层
- [完整链条回顾](../24_Complete_Chain/) - 知识整合

---

*模块创建: 2026-03-29*
