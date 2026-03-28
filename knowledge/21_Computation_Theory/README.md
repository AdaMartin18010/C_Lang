# 计算理论基础

> **模块定位**: 知识链条起点 - 从理论到物理的桥梁
> **核心使命**: 理解"什么是可计算的"以及"为什么物理计算机能实现计算"
> **预计时间**: 2个月
> **模块状态**: Month 1 进行中

---

## 模块概述

本模块回答计算机科学最根本的问题：**什么是可计算的？**

### 为什么需要这一层？

在深入物理实现（晶体管、CPU）之前，我们必须理解计算的理论边界。这不仅是学术兴趣，更是为了建立**从数学抽象到物理实现的认知桥梁**。

```
认知链条:
图灵机（数学模型）
    ↓
为什么它能被物理实现？（Church-Turing物理论题）
    ↓
布尔函数 → 逻辑门 → CPU（您的已有知识）
    ↓
汇编语言（机器语义）
    ↓
C语言（高级抽象）
```

---

## 文件结构

### Month 1: 计算基础

| 文件 | 内容 | 状态 | 与现有内容连接 |
|:-----|:-----|:----:|:---------------|
| [01_Turing_Machines.md](./01_Turing_Machines.md) | 图灵机：计算的理论模型 | ✅ 完成 | → 数字逻辑基础 |
| 02_Lambda_Calculus.md | λ演算：函数的视角 | 📝 待创建 | → 函数式编程概念 |
| 03_Computational_Complexity.md | 计算复杂性：P vs NP | 📝 待创建 | → 算法分析 |
| 04_Theory_to_Physical_Bridge.md | 理论到物理的桥梁 ⭐ | 📝 待创建 | → 所有物理层内容 |

### Month 2: 汇编与语义

| 文件 | 内容 | 状态 | 与现有内容连接 |
|:-----|:-----|:----:|:---------------|
| ../22_Assembler_Implementation/01_Hack_Assembler.md | Hack汇编器实现 | 📝 待创建 | → ISA层 |
| ../22_Assembler_Implementation/02_RISCV_Assembler.md | RISC-V汇编器 | 📝 待创建 | → RISC-V CPU实现 |
| ../22_Assembler_Implementation/03_C_to_Assembly_Data.md | C到汇编：数据表示 | 📝 待创建 | → 指针深度 |
| ../22_Assembler_Implementation/04_C_to_Assembly_Control.md | C到汇编：控制流 | 📝 待创建 | → 函数调用约定 |

### Month 3-4: 虚拟机与解释器

| 文件 | 内容 | 状态 | 与现有内容连接 |
|:-----|:-----|:----:|:---------------|
| ../23_VM_Implementation/01_Stack_Based_VM.md | 栈式虚拟机 | 📝 待创建 | → 现有VM内容 |
| ../23_VM_Implementation/02_C_Subset_Interpreter.md | C子集解释器 | 📝 待创建 | → C语言核心 |

### Month 5-6: 整合回顾

| 文件 | 内容 | 状态 |
|:-----|:-----|:----:|
| ../24_Complete_Chain/01_From_Turing_to_C.md | 完整链条回顾 | 📝 待创建 |

---

## 当前进度

```
Month 1 进度:
[████████░░░░░░░░░░░░] 25%

已完成:
✅ 目录结构创建
✅ 01_Turing_Machines.md 文档
✅ 图灵机模拟器代码

进行中:
📝 与现有内容的连接标注

待开始:
⏳ λ演算
⏳ 复杂性理论
⏳ 理论-物理桥梁 ⭐
```

---

## 学习路径建议

### 如果您是第一次学习计算理论

```
推荐顺序:
1. 阅读 01_Turing_Machines.md
2. 运行图灵机模拟器，手工模拟几个例子
3. 思考"与物理层的连接"部分
4. 阅读现有物理层内容，寻找对应关系
5. 记录您的理解和疑问
6. 继续 λ演算
```

### 如果您已有计算理论基础

```
快速路径:
1. 浏览 01_Turing_Machines.md 检查理解
2. 重点阅读"与现有知识库的连接"部分
3. 直接跳到 04_Theory_to_Physical_Bridge.md（创建中）
4. 从汇编器实现开始实践
```

---

## 关键连接点

本模块的核心价值在于与现有内容的**连接**：

| 理论概念 | 物理实现 | 连接文档 |
|:---------|:---------|:---------|
| 图灵机状态 | CPU状态寄存器 | 01_Turing_Machines.md → 09_Physical_Machine_Layer |
| λ演算函数 | C函数调用 | 02_Lambda_Calculus.md → 01_Core/函数与作用域 |
| 汇编语义 | ISA指令 | 22_Assembler → 10_ISA_Machine_Code |
| VM字节码 | 物理机器码 | 23_VM → 10_ISA_Machine_Code |
| C语义 | 汇编代码 | 24_Complete_Chain → 所有层 |

---

## 下一步行动

### 立即开始

1. **阅读**: [01_Turing_Machines.md](./01_Turing_Machines.md)
2. **运行**: `python examples/turing_machine/tm_simulator.py`
3. **思考**: 图灵机如何映射到您的RISC-V CPU实现？

### 本周目标

- [ ] 完成图灵机文档阅读
- [ ] 运行并理解模拟器代码
- [ ] 在现有物理层文档中添加至少3个指向本模块的链接
- [ ] 准备λ演算的学习材料

---

*模块创建: 2026-03-29*
*下次更新: 完成λ演算文档后*
