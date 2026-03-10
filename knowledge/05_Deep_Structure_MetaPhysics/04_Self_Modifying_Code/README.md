# 自修改代码 (Self-Modifying Code)

## 目录概述

本目录深入探讨**自修改代码(Self-Modifying Code, SMC)**的理论与技术，涵盖JIT(Just-In-Time)编译基础、追踪JIT优化以及二进制翻译三大主题。
从运行时代码生成视角重新审视程序执行模型，探讨动态编译技术在虚拟机、模拟器和性能优化中的核心应用。

---

## 理论背景

### 自修改代码的数学本质

自修改代码突破了传统**静态程序**的假设，引入**元循环(Meta-circular)**执行模型：

**定义 (自修改程序)**:
程序 $P$ 是一个四元组 $P = (C_0, D, T, \Rightarrow)$，其中：

- $C_0$：初始代码段
- $D$：数据段（可能包含代码模板）
- $T: C \times D \rightarrow C$：代码变换函数
- $\Rightarrow \subseteq State \times State$：执行转换关系

程序执行的**双层级结构**：

```text
Level 1: 元级别 (Meta-level)
    ┌───────────────────────┐
    │   代码生成/优化引擎    │
    │   (JIT Compiler)      │
    └───────────┬───────────┘
                ↓ 生成
Level 0: 对象级别 (Object-level)
    ┌───────────────────────┐
    │    执行代码 (IR/机器码) │
    │    (Generated Code)   │
    └───────────────────────┘
```

### JIT编译理论基础

**Partial Evaluation (部分求值)**:
Futuamura投影建立了编译与解释的关系：

**第一投影** (解释器编译化):
$$target\_program = [[mix]](interpreter, source\_program)$$

**第二投影** (编译器生成):
$$compiler = [[mix]](mix, interpreter)$$

**第三投影** (编译器生成器):
$$cogen = [[mix]](mix, mix)$$

**Futamura三角**:

```text
         源程序 + 输入
              ↓
    解释器 → 输出
       ↓
    mix + 解释器 → 目标程序
       ↓
    mix + mix → 编译器生成器
```

### 追踪JIT的形式化

**追踪(Trace)**：热点路径的线性指令序列

**定义 (追踪区域)**:
给定控制流图 $G = (V, E)$，追踪 $\tau$ 是路径 $v_0 \rightarrow v_1 \rightarrow \cdots \rightarrow v_n$，满足：

- $v_0$ 是**追踪入口(Trace Head)**：入边来自区域外部
- $v_n$ 是**追踪出口(Trace Exit)**：出边指向区域外部

**旁路计数器(Side Exit)**:
追踪假设失败时的回退点，形式化为**条件跳转**的补集：
$$side\_exit: \neg guard \rightarrow \text{interpreter}$$

### 二进制翻译的范畴论视角

**源-目标语义保持**:
二进制翻译器 $T$ 是从源架构语义范畴 $\mathcal{S}$ 到目标架构语义范畴 $\mathcal{T}$ 的**忠实函子(Faithful Functor)**：

$$T: \mathcal{S} \rightarrow \mathcal{T}$$

**正确性条件**:
$$\forall p \in Source, \llbracket T(p) \rrbracket_{\mathcal{T}} \cong \llbracket p \rrbracket_{\mathcal{S}}$$

### 动态优化理论

**猜测-验证模式**:

```text
观察(Profiling) → 假设(Speculation) → 优化(Optimization) → 验证(Validation)
       ↓                  ↓                  ↓                 ↓
   热点识别         类型/路径猜测       特化代码生成      保护检查(Guard)
```

**去优化(Deoptimization)**:
当假设失败时，从优化代码安全回退到未优化代码的**双射映射**。

---

## 文件结构

| 文件名 | 难度 | 核心内容 |
|--------|------|----------|
| `01_JIT_Basics.md` | ⭐⭐⭐⭐ | JIT编译器架构，分层编译，代码缓存管理，部分求值理论，内联缓存 |
| `02_Tracing_JIT.md` | ⭐⭐⭐⭐⭐ | 追踪记录与编译，循环优化，旁路处理，类型特化， guard 条件生成 |
| `03_Binary_Translation.md` | ⭐⭐⭐⭐⭐ | 静态与动态二进制翻译，指令集模拟，内存模型转换，系统态模拟 |

---

## 前置知识

- **编译原理**: 代码生成、寄存器分配、优化 pass
- **计算机体系结构**: 指令集架构、流水线、缓存层次
- **操作系统**: 虚拟内存、代码页保护、信号处理
- **运行时系统**: 垃圾回收、栈展开、异常处理

---

## 学术参考文献

### JIT编译与部分求值

1. **Aycock, J.** (2003). *A Brief History of Just-In-Time*. ACM Computing
   Surveys, 35(2), 97-113. (JIT技术发展综述)

2. **Jones, N. D., Gomard, C. K., & Sestoft, P.** (1993). *Partial Evaluation
   and Automatic Program Generation*. Prentice Hall. (部分求值经典专著)

3. **Futamura, Y.** (1999). *Partial Evaluation of Computation Process — An
   Approach to a Compiler-Compiler*. Higher-Order and Symbolic Computation,
   12(4), 381-391. (Futamura投影原始论文重印)

4. **Holzle, U., Ungar, D., et al.** (1994). *Optimizing Dynamically-Dispatched
   Calls with Run-Time Type Feedback*. In PLDI '94. (内联缓存奠基)

### 追踪JIT

1. **Gal, A., et al.** (2009). *Trace-Based Just-In-Time Type Specialization
   for Dynamic Languages*. In PLDI '09. (TraceMonkey，现代追踪JIT奠基)

2. **Bebenita, M., et al.** (2010). *SPUR: A Trace-Based JIT Compiler for CIL*.
   In OOPSLA '10. (面向CLI的追踪JIT)

3. **Bala, V., Duesterwald, E., & Banerjia, S.** (2000). *Dynamo: A Transparent
   Dynamic Optimization System*. In PLDI '00. (HP Dynamo，动态优化先驱)

4. **Bolz, C. F., Cuni, A., Fijalkowski, M., & Rigo, A.** (2009).
   *Tracing the Meta-Level: PyPy's Tracing JIT Compiler*. In ICOOOLPS '09.
   (PyPy元追踪技术)

### 二进制翻译

1. **Sites, R. L., Chernoff, A., et al.** (1993). *Binary Translation*.
   Communications of the ACM, 36(2), 69-81. (DEC FX!32早期工作)

2. **Bala, V., Duesterwald, E., & Banerjia, S.** (2000). *Transparent
    Dynamic Optimization: The Design and Implementation of Dynamo*.
    Technical Report, HP Labs. (动态二进制翻译)

3. **Bruening, D., Garnett, T., & Amarasinghe, S.** (2003). *An
    Infrastructure for Adaptive Dynamic Optimization*. In CGO '03.
    (DynamoRIO基础)

4. **Luk, C. K., et al.** (2005). *Pin: Building Customized Program Analysis
    Tools with Dynamic Instrumentation*. In PLDI '05. (Pin框架)

### 虚拟机与运行时

1. **Palem, G. V., & Simons, B.** (1993). *Instruction Scheduling for
    Self-Modifying Code*. In ASPLOS '93. (自修改代码调度)

2. **Cheney, A. E.** (2002). *A Formalization of Self-Modifying Code*.
    Technical Report, MIT. (SMC形式化)

3. **Ansel, J., et al.** (2014). *Language-Defined Virtual Memory for
    Safe, Efficient, Paged Memory Allocation*. In OOPSLA '14.
    (现代运行时内存管理)

### 高级优化

1. **Würthinger, T., et al.** (2013). *Self-Optimizing AST Interpreters*.
    In DLS '13. (Graal/Truffle优化)

2. **Würthinger, T., et al.** (2017). *Practical Partial Evaluation for
    High-Performance Dynamic Language Runtimes*. In PLDI '17.
    (GraalVM的PE应用)

---

## 学习路径建议

```text
编译基础 → 运行时系统 → JIT基础 → 追踪JIT → 二进制翻译 → 虚拟机优化
    ↓            ↓           ↓          ↓            ↓            ↓
 代码生成      内存管理    分层编译   路径优化    指令模拟    元循环优化
```

---

## 关键技术指标

| 技术 | 启动延迟 | 峰值性能 | 适用场景 |
|------|---------|---------|----------|
| 解释器 | 极低 | 低 | 冷代码、调试 |
| 方法JIT | 中 | 高 | 稳定热点方法 |
| 追踪JIT | 低 | 极高 | 循环密集型 |
| AOT编译 | 无 | 高 | 启动敏感 |
| 分层编译 | 低 | 极高 | 通用场景 |

---

## 安全与防护

### W⊕X (Write XOR Execute)

现代操作系统通过内存保护防止恶意自修改：

- 代码页可执行但不可写
- 数据页可写但不可执行

**合法JIT代码生成**需要：

1. 分配可读写的内存区域
2. 写入机器代码
3. 调用 `mprotect()` 设置为可执行
4. 刷新指令缓存 (cache flush)

### 安全威胁模型

| 威胁 | 机制 | 防护 |
|------|------|------|
| 代码注入 | 缓冲区溢出覆写代码 | DEP/NX bit |
| 返回导向编程 | 重用现有代码片段 | ASLR、CFI |
| JIT喷射 | 预测JIT输出模式 | 常数致盲 |

---

## 相关链接

- [父目录: 深层结构与元物理学](../README.md)
- [形式语义学](../01_Formal_Semantics/README.md)
- [C到汇编映射](../../02_Formal_Semantics_and_Physics/06_C_Assembly_Mapping/README.md)

---

## 数学符号速查

| 符号 | 含义 |
|------|------|
| $[[mix]]$ | 部分求值混合器 |
| $\tau$ | 追踪(Trace) |
| $\phi$ | 代码变换函数 |
| $T: \mathcal{S} \rightarrow \mathcal{T}$ | 二进制翻译函子 |
| $C_0$ | 初始代码段 |
| $guard$ | 保护条件 |
| $\llbracket P \rrbracket$ | 程序语义 |
| $\Rightarrow$ | 执行转换 |

---

*本目录内容适合虚拟机开发者、运行时系统工程师及高级编译器优化研究者阅读。*

*最后更新: 2026-03-10*
