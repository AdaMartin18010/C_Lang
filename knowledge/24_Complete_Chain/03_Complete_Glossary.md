# 完整术语表与学习路径指南

> **层级定位**: 24 Complete Chain / 参考层
> **前置知识**: [从图灵机到C](./01_From_Turing_to_C.md), [层间桥梁](./02_Layer_Bridges.md)
> **横向关联**: [全局索引](../06_Thinking_Representation/08_Index/01_Global_Index.md), [核心概念索引](../06_Thinking_Representation/06_Index/01_Core_Concepts_Index.md)
> **预计学习时间**: 持续参考

---

## 核心洞察

```
本文档是"从图灵机到C语言"知识链条的完整参考手册：

1. 术语对照表：按层次组织的核心概念
2. 概念索引：快速查找概念定义
3. 学习路径：推荐的阅读顺序
4. 资源汇总：扩展阅读和学习材料

作为学习者，你可以：
- 在学习过程中快速查阅术语
- 按推荐路径系统学习
- 发现概念间的隐藏联系
```

---

## 1. 按层次组织的术语表

### 1.1 Layer 1: 计算理论层

| 术语 | 英文 | 定义 | 相关文档 |
|:-----|:-----|:-----|:---------|
| 图灵机 | Turing Machine | 计算的数学模型，由无限磁带、读写头和有限状态控制器组成 | [01_Turing_Machines.md](../21_Computation_Theory/01_Turing_Machines.md) |
| 可计算性 | Computability | 一个问题如果可以被算法解决，则称它是可计算的 | [01_Turing_Machines.md](../21_Computation_Theory/01_Turing_Machines.md) |
| 丘奇-图灵论题 | Church-Turing Thesis | 任何可计算的函数都可以用图灵机计算 | [04_Theory_to_Physical_Bridge.md](../21_Computation_Theory/04_Theory_to_Physical_Bridge.md) |
| λ演算 | Lambda Calculus | 基于函数抽象和应用的计算模型 | [02_Lambda_Calculus.md](../21_Computation_Theory/02_Lambda_Calculus.md) |
| 判定器 | Decider | 对所有输入都停机的图灵机 | [01_Turing_Machines.md](../21_Computation_Theory/01_Turing_Machines.md) |
| 识别器 | Recognizer | 对接受输入停机，对拒绝输入可能不停机的图灵机 | [01_Turing_Machines.md](../21_Computation_Theory/01_Turing_Machines.md) |
| 配置 | Configuration | 图灵机某一时刻的完整状态（状态+磁带+磁头位置） | [01_Turing_Machines.md](../21_Computation_Theory/01_Turing_Machines.md) |
| P vs NP | P vs NP | 可在多项式时间验证的问题是否都能在多项式时间解决 | [03_Computational_Complexity.md](../21_Computation_Theory/03_Computational_Complexity.md) |

### 1.2 Layer 2: 数字逻辑层

| 术语 | 英文 | 定义 | 相关文档 |
|:-----|:-----|:-----|:---------|
| 布尔代数 | Boolean Algebra | 处理真值（0和1）的代数系统 | [数字逻辑基础](../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/01_Digital_Logic_Foundations/README.md) |
| 逻辑门 | Logic Gate | 实现布尔函数的基本电路单元 | [Logic_Gates](../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/01_Digital_Logic_Foundations/02_Logic_Gates_Implementation.md) |
| 组合逻辑 | Combinational Logic | 输出仅依赖当前输入的逻辑电路 | [Combinational_Circuits](../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/01_Digital_Logic_Foundations/03_Combinational_Circuits.md) |
| 时序逻辑 | Sequential Logic | 输出依赖当前输入和历史状态的逻辑电路 | [SR_Latch](../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/03_Memory_Circuits/01_SR_Latch_and_D_FlipFlop.md) |
| 触发器 | Flip-Flop | 存储1位数据的时序电路元件 | [D_FlipFlop](../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/03_Memory_Circuits/01_SR_Latch_and_D_FlipFlop.md) |
| ALU | Arithmetic Logic Unit | 执行算术和逻辑运算的电路 | [ALU_Design](../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/02_Arithmetic_Logic_Unit/03_ALU_Design.md) |
| 加法器 | Adder | 执行加法运算的电路 | [Binary_Arithmetic](../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/02_Arithmetic_Logic_Unit/01_Binary_Arithmetic.md) |
| 多路选择器 | Multiplexer | 从多个输入中选择一个输出的电路 | [组合电路](../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/01_Digital_Logic_Foundations/03_Combinational_Circuits.md) |
| SRAM | Static RAM | 使用触发器存储数据的静态随机存取存储器 | [SRAM_Cell](../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/03_Memory_Circuits/03_SRAM_Cell.md) |

### 1.3 Layer 3: CPU/ISA层

| 术语 | 英文 | 定义 | 相关文档 |
|:-----|:-----|:-----|:---------|
| ISA | Instruction Set Architecture | 软件与硬件之间的接口规范 | [ISA](../02_Formal_Semantics_and_Physics/10_ISA_Machine_Code/01_Instruction_Set_Architecture.md) |
| RISC | Reduced Instruction Set Computer | 精简指令集计算机，指令简单规整 | [RISC-V_CPU](../02_Formal_Semantics_and_Physics/10_ISA_Machine_Code/08_RISCV_CPU_Complete/README.md) |
| CISC | Complex Instruction Set Computer | 复杂指令集计算机，指令功能丰富 | [x86_Microarchitecture](../02_Formal_Semantics_and_Physics/10_ISA_Machine_Code/12_x86_Microarchitecture_Impl.md) |
| 程序计数器 | Program Counter (PC) | 存储下一条要执行指令地址的寄存器 | [RV32I_Single_Cycle](../02_Formal_Semantics_and_Physics/10_ISA_Machine_Code/08_RISCV_CPU_Complete/01_RV32I_Single_Cycle.md) |
| 寄存器堆 | Register File | CPU内部的高速存储单元集合 | [Register_File](../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/03_Memory_Circuits/02_Register_File.md) |
| 流水线 | Pipeline | 重叠执行多条指令的技术 | [RV32I_Pipelined](../02_Formal_Semantics_and_Physics/10_ISA_Machine_Code/08_RISCV_CPU_Complete/02_RV32I_Pipelined.md) |
| 冒险 | Hazard | 流水线中导致指令执行顺序问题的冲突 | [RV32I_Pipelined](../02_Formal_Semantics_and_Physics/10_ISA_Machine_Code/08_RISCV_CPU_Complete/02_RV32I_Pipelined.md) |
| 中断 | Interrupt | 打断CPU正常执行流程的外部信号 | [Interrupt_Controller](../02_Formal_Semantics_and_Physics/10_ISA_Machine_Code/09_Interrupt_Hardware/01_Interrupt_Controller.md) |
| 系统调用 | System Call | 用户程序请求操作系统服务的机制 | [System_Call](../02_Formal_Semantics_and_Physics/10_ISA_Machine_Code/09_Interrupt_Hardware/02_System_Call_Hardware.md) |
| MMU | Memory Management Unit | 虚拟地址到物理地址转换的硬件 | [MMU_Architecture](../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/05_Memory_Management_Hardware/01_MMU_Architecture.md) |
| TLB | Translation Lookaside Buffer | 地址转换缓存 | [TLB](../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/05_Memory_Management_Hardware/03_TLB_Implementation.md) |
| Cache | Cache | 高速缓冲存储器 | [Cache_Array](../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/03_Memory_Circuits/04_Cache_Array.md) |

### 1.4 Layer 4: 汇编层

| 术语 | 英文 | 定义 | 相关文档 |
|:-----|:-----|:-----|:---------|
| 汇编器 | Assembler | 将汇编语言翻译为机器码的程序 | [Hack汇编器](../22_Assembler_Implementation/01_Hack_Assembler.md), [RISC-V汇编器](../22_Assembler_Implementation/02_RISCV_Assembler.md) |
| 指令格式 | Instruction Format | 机器指令的二进制编码结构 | [02_RISCV_Assembler.md](../22_Assembler_Implementation/02_RISCV_Assembler.md) |
| 操作码 | Opcode | 标识指令类型的字段 | [02_RISCV_Assembler.md](../22_Assembler_Implementation/02_RISCV_Assembler.md) |
| 寻址模式 | Addressing Mode | 确定操作数位置的方式 | [02_RISCV_Assembler.md](../22_Assembler_Implementation/02_RISCV_Assembler.md) |
| 伪指令 | Pseudo-instruction | 汇编器展开为真实指令的语法糖 | [02_RISCV_Assembler.md](../22_Assembler_Implementation/02_RISCV_Assembler.md) |
| 标签 | Label | 代表地址的符号名 | [01_Hack_Assembler.md](../22_Assembler_Implementation/01_Hack_Assembler.md) |
| 符号表 | Symbol Table | 符号名到地址的映射表 | [01_Hack_Assembler.md](../22_Assembler_Implementation/01_Hack_Assembler.md) |
| 重定位 | Relocation | 链接时调整地址引用的过程 | [02_RISCV_Assembler.md](../22_Assembler_Implementation/02_RISCV_Assembler.md) |
| 调用约定 | Calling Convention | 函数调用的参数传递和寄存器使用规则 | [04_C_to_Assembly_Control.md](../22_Assembler_Implementation/04_C_to_Assembly_Control.md) |
| 栈帧 | Stack Frame | 函数调用时在栈上分配的内存区域 | [04_C_to_Assembly_Control.md](../22_Assembler_Implementation/04_C_to_Assembly_Control.md) |
| ABI | Application Binary Interface | 应用程序二进制接口规范 | [调用约定](../01_Core_Knowledge_System/25_C_Interfacing/01_Calling_Conventions.md) |

### 1.5 Layer 5: 虚拟机层

| 术语 | 英文 | 定义 | 相关文档 |
|:-----|:-----|:-----|:---------|
| 虚拟机 | Virtual Machine | 在软件中模拟的计算环境 | [栈式VM](../23_VM_Implementation/01_Stack_Based_VM.md) |
| 字节码 | Bytecode | 虚拟机执行的中间指令 | [01_Stack_Based_VM.md](../23_VM_Implementation/01_Stack_Based_VM.md) |
| 操作数栈 | Operand Stack | 栈式VM中存储操作数的数据结构 | [01_Stack_Based_VM.md](../23_VM_Implementation/01_Stack_Based_VM.md) |
| 解释器 | Interpreter | 直接执行源代码或中间代码的程序 | [02_C_Subset_Interpreter.md](../23_VM_Implementation/02_C_Subset_Interpreter.md) |
| JIT编译 | Just-In-Time Compilation | 运行时编译字节码为机器码 | [JIT](../03_System_Technology_Domains/01_Virtual_Machine_Interpreter/03_JIT_Compilation.md) |
| 词法分析 | Lexical Analysis | 将字符流转换为Token流的过程 | [02_C_Subset_Interpreter.md](../23_VM_Implementation/02_C_Subset_Interpreter.md) |
| 语法分析 | Syntax Analysis | 将Token流转换为语法树的过程 | [02_C_Subset_Interpreter.md](../23_VM_Implementation/02_C_Subset_Interpreter.md) |
| 递归下降 | Recursive Descent | 手工实现语法分析的方法 | [02_C_Subset_Interpreter.md](../23_VM_Implementation/02_C_Subset_Interpreter.md) |
| AST | Abstract Syntax Tree | 源代码的抽象语法树表示 | [02_C_Subset_Interpreter.md](../23_VM_Implementation/02_C_Subset_Interpreter.md) |
| 垃圾回收 | Garbage Collection | 自动回收不再使用的内存 | [Garbage_Collection](../03_System_Technology_Domains/04_Garbage_Collection.md) |

### 1.6 Layer 6: C语言层

| 术语 | 英文 | 定义 | 相关文档 |
|:-----|:-----|:-----|:---------|
| 类型系统 | Type System | 对数据分类和约束的规则 | [02_Data_Type_System.md](../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) |
| 指针 | Pointer | 存储内存地址的变量 | [01_Pointer_Depth.md](../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) |
| 数组 | Array | 相同类型元素的连续内存块 | [05_Arrays_Pointers.md](../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) |
| 结构体 | Structure | 不同类型成员的复合数据类型 | [01_Structures_Unions.md](../01_Core_Knowledge_System/03_Construction_Layer/01_Structures_Unions.md) |
| 联合体 | Union | 成员共享内存的复合类型 | [01_Structures_Unions.md](../01_Core_Knowledge_System/03_Construction_Layer/01_Structures_Unions.md) |
| 函数 | Function | 可重复使用的代码块 | [04_Functions_Scope.md](../01_Core_Knowledge_System/02_Core_Layer/04_Functions_Scope.md) |
| 作用域 | Scope | 标识符的可见范围 | [04_Functions_Scope.md](../01_Core_Knowledge_System/02_Core_Layer/04_Functions_Scope.md) |
| 链接 | Linkage | 标识符在不同翻译单元间的可见性 | [03_Modularization_Linking.md](../01_Core_Knowledge_System/03_Construction_Layer/03_Modularization_Linking.md) |
| 存储期 | Storage Duration | 变量在内存中存在的时间 | [02_Memory_Management.md](../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) |
| 未定义行为 | Undefined Behavior | 标准未规定的行为 | [02_Undefined_Behavior.md](../01_Core_Knowledge_System/06_Advanced_Layer/02_Undefined_Behavior.md) |
| 内存对齐 | Memory Alignment | 数据在内存中的地址对齐要求 | [03_C_to_Assembly_Data.md](../22_Assembler_Implementation/03_C_to_Assembly_Data.md) |

---

## 2. 概念索引

### 2.1 按主题索引

**计算理论**

- 图灵机 → [01_Turing_Machines.md](../21_Computation_Theory/01_Turing_Machines.md)
- λ演算 → [02_Lambda_Calculus.md](../21_Computation_Theory/02_Lambda_Calculus.md)
- 复杂性 → [03_Computational_Complexity.md](../21_Computation_Theory/03_Computational_Complexity.md)
- 理论与物理桥梁 → [04_Theory_to_Physical_Bridge.md](../21_Computation_Theory/04_Theory_to_Physical_Bridge.md)

**硬件实现**

- 数字逻辑基础 → [01_Digital_Logic_Foundations](../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/01_Digital_Logic_Foundations/README.md)
- ALU设计 → [02_Arithmetic_Logic_Unit](../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/02_Arithmetic_Logic_Unit/README.md)
- 内存电路 → [03_Memory_Circuits](../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/03_Memory_Circuits/README.md)
- RISC-V CPU → [08_RISCV_CPU_Complete](../02_Formal_Semantics_and_Physics/10_ISA_Machine_Code/08_RISCV_CPU_Complete/README.md)

**汇编与底层**

- Hack汇编器 → [01_Hack_Assembler.md](../22_Assembler_Implementation/01_Hack_Assembler.md)
- RISC-V汇编器 → [02_RISCV_Assembler.md](../22_Assembler_Implementation/02_RISCV_Assembler.md)
- C到汇编（数据） → [03_C_to_Assembly_Data.md](../22_Assembler_Implementation/03_C_to_Assembly_Data.md)
- C到汇编（控制流） → [04_C_to_Assembly_Control.md](../22_Assembler_Implementation/04_C_to_Assembly_Control.md)

**虚拟机与解释器**

- 栈式VM → [01_Stack_Based_VM.md](../23_VM_Implementation/01_Stack_Based_VM.md)
- C子集解释器 → [02_C_Subset_Interpreter.md](../23_VM_Implementation/02_C_Subset_Interpreter.md)

**整合回顾**

- 完整链条回顾 → [01_From_Turing_to_C.md](./01_From_Turing_to_C.md)
- 层间桥梁 → [02_Layer_Bridges.md](./02_Layer_Bridges.md)
- 术语表（本文档） → [03_Complete_Glossary.md](./03_Complete_Glossary.md)

### 2.2 跨层概念映射

```
跨层概念对应表:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

概念            理论层      物理层      汇编层      C层
─────────────────────────────────────────────────────
状态            图灵机状态  寄存器      寄存器      变量
存储            磁带        内存        内存        变量/对象
计算            转移函数    ALU         指令        表达式
控制流          状态转换    PC+跳转     跳转指令    if/for/while
子程序          -           调用指令    call/ret    函数调用
参数传递        -           寄存器+栈   寄存器+栈   函数参数
局部变量        -           栈帧        栈帧        局部变量
全局变量        -           数据段      标签        全局变量

查找路径:
• 想了解C指针的物理实现？
  → [03_C_to_Assembly_Data.md](../22_Assembler_Implementation/03_C_to_Assembly_Data.md)
  → [01_Pointer_Depth.md](../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md)

• 想了解函数调用的底层机制？
  → [04_C_to_Assembly_Control.md](../22_Assembler_Implementation/04_C_to_Assembly_Control.md)
  → [调用约定](../01_Core_Knowledge_System/25_C_Interfacing/01_Calling_Conventions.md)

• 想了解虚拟机的实现？
  → [01_Stack_Based_VM.md](../23_VM_Implementation/01_Stack_Based_VM.md)
  → [02_C_Subset_Interpreter.md](../23_VM_Implementation/02_C_Subset_Interpreter.md)
```

---

## 3. 推荐学习路径

### 3.1 完整学习路径（6个月）

```
Month 1: 计算理论基础
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Week 1-2: 图灵机
  ├── 阅读 [01_Turing_Machines.md]
  └── 实现图灵机模拟器

Week 3: λ演算
  ├── 阅读 [02_Lambda_Calculus.md]
  └── 练习λ表达式求值

Week 4: 理论到物理桥梁
  ├── 阅读 [04_Theory_to_Physical_Bridge.md]
  └── 思考图灵机与物理计算机的对应

Month 2: 汇编器实现
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Week 1-2: Hack汇编器
  ├── 阅读 [01_Hack_Assembler.md]
  └── 实现Hack汇编器

Week 3-4: RISC-V汇编器
  ├── 阅读 [02_RISCV_Assembler.md]
  └── 实现RISC-V汇编器

Month 3: C到汇编映射
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Week 1-2: 数据表示
  ├── 阅读 [03_C_to_Assembly_Data.md]
  └── 分析C结构体的汇编输出

Week 3-4: 控制流
  ├── 阅读 [04_C_to_Assembly_Control.md]
  └── 追踪函数调用的栈帧变化

Month 4: 虚拟机
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Week 1-2: 栈式VM
  ├── 阅读 [01_Stack_Based_VM.md]
  └── 实现栈式虚拟机

Week 3-4: 解释器
  ├── 阅读 [02_C_Subset_Interpreter.md]
  └── 研究C4源代码

Month 5-6: 整合回顾
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Week 1-2: 完整链条回顾
  ├── 阅读 [01_From_Turing_to_C.md]
  └── 绘制自己的知识图谱

Week 3-4: 层间桥梁
  ├── 阅读 [02_Layer_Bridges.md]
  └── 手工编写完整编译链条

Week 5-6: 实践项目
  └── 完成一个综合性项目
```

### 3.2 快速参考路径

```
已有基础，快速查漏补缺:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

计算机科学背景:
├── 已有计算理论基础
│   └── 跳过 Month 1，直接看 [04_Theory_to_Physical_Bridge.md]
│
├── 已有汇编经验
│   └── 快速浏览 [02_RISCV_Assembler.md]
│   └── 重点阅读 C到汇编映射部分
│
└── 已有编译器经验
    └── 快速浏览 [01_Stack_Based_VM.md]
    └── 研究 [02_C_Subset_Interpreter.md]

软件工程师:
├── 想理解底层实现
│   └── [03_C_to_Assembly_Data.md]
│   └── [04_C_to_Assembly_Control.md]
│   └── [02_Layer_Bridges.md]
│
└── 想设计语言/VM
    └── [01_Stack_Based_VM.md]
    └── [02_C_Subset_Interpreter.md]

硬件工程师:
└── 想理解软件视角
    └── [01_From_Turing_to_C.md]
    └── [03_C_to_Assembly_Data.md]
```

### 3.3 主题专项路径

```
主题：函数调用深度理解
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
1. [04_C_to_Assembly_Control.md] - 函数调用汇编实现
2. [调用约定](../01_Core_Knowledge_System/25_C_Interfacing/01_Calling_Conventions.md) - ABI规范
3. [02_Layer_Bridges.md] - 形式化映射
4. 实践：用gdb跟踪函数调用

主题：类型系统实现
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
1. [02_Data_Type_System.md] - C类型系统
2. [03_C_to_Assembly_Data.md] - 类型到汇编的映射
3. [02_C_Subset_Interpreter.md] - 解释器中的类型处理
4. 实践：实现一个简单类型检查器

主题：内存管理全景
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
1. [02_Memory_Management.md] - C内存管理
2. [03_C_to_Assembly_Data.md] - 内存布局
3. [MMU_Architecture](../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/05_Memory_Management_Hardware/01_MMU_Architecture.md) - 硬件支持
4. 实践：实现一个简单的malloc
```

---

## 4. 学习资源汇总

### 4.1 核心文档

**必读（按顺序）**

1. [01_Turing_Machines.md](../21_Computation_Theory/01_Turing_Machines.md) - 理论基础
2. [04_Theory_to_Physical_Bridge.md](../21_Computation_Theory/04_Theory_to_Physical_Bridge.md) - 理论到物理
3. [01_From_Turing_to_C.md](./01_From_Turing_to_C.md) - 完整回顾
4. [02_Layer_Bridges.md](./02_Layer_Bridges.md) - 精确映射

**汇编层**

- [01_Hack_Assembler.md](../22_Assembler_Implementation/01_Hack_Assembler.md)
- [02_RISCV_Assembler.md](../22_Assembler_Implementation/02_RISCV_Assembler.md)
- [03_C_to_Assembly_Data.md](../22_Assembler_Implementation/03_C_to_Assembly_Data.md)
- [04_C_to_Assembly_Control.md](../22_Assembler_Implementation/04_C_to_Assembly_Control.md)

**虚拟机层**

- [01_Stack_Based_VM.md](../23_VM_Implementation/01_Stack_Based_VM.md)
- [02_C_Subset_Interpreter.md](../23_VM_Implementation/02_C_Subset_Interpreter.md)

### 4.2 扩展阅读

**书籍**

- 《计算机系统要素》(Nand2Tetris) - 从逻辑门到完整计算机
- 《深入理解计算机系统》(CS:APP) - 从程序员视角理解系统
- 《编译器设计》(Dragon Book) - 编译原理权威
- 《计算机组成与设计》(Patterson & Hennessy) - CPU设计

**在线资源**

- [RISC-V官方规范](https://riscv.org/technical/specifications/)
- [Nand2Tetris课程](https://www.nand2tetris.org/)
- [C4源代码](https://github.com/rswier/c4)

### 4.3 实践项目

**入门项目**

1. 实现图灵机模拟器
2. 实现Hack汇编器
3. 手工将C程序翻译为汇编

**进阶项目**

1. 实现RISC-V汇编器
2. 实现栈式虚拟机
3. 扩展C4解释器

**综合项目**

1. 完整的编译器：自定义语言 → 汇编
2. 玩具操作系统：从启动到简单shell
3. FPGA上的RISC-V CPU实现

---

## 5. 检查清单

### 5.1 学习完成检查

```
理论基础 (Month 1)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
□ 理解图灵机的形式化定义
□ 能手工模拟简单图灵机的执行
□ 理解丘奇-图灵论题
□ 理解λ演算的基本概念
□ 能解释计算理论的物理含义

汇编实现 (Month 2)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
□ 理解RISC-V指令格式
□ 能实现基本的汇编器
□ 理解C数据类型的汇编表示
□ 理解C控制流的汇编实现
□ 能手工编写C程序的汇编版本

虚拟机 (Month 3)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
□ 理解栈式VM的架构
□ 能实现基本的栈式VM
□ 理解词法分析和语法分析
□ 能实现简单的递归下降解析器
□ 理解从C到字节码的翻译

整合 (Month 4-6)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
□ 能描述从图灵机到C的完整链条
□ 理解各层之间的精确映射
□ 能在不同抽象层次间切换思维
□ 完成至少一个综合实践项目
```

---

## 6. 反馈与贡献

本文档是"从图灵机到C语言"知识链条的一部分。如果你发现：

- 术语定义需要修正
- 学习路径需要调整
- 相关资源需要补充

欢迎提交反馈。

---

*文档创建: 2026-03-29*
*上次更新: 2026-03-29*
*状态: 核心框架完成*
