# 编译器后端与代码生成

> **层级定位**: 02 Formal Semantics and Physics / 11 Compiler Internals
> **参考课程**: Stanford CS143, CMU 15-411, MIT 6.035
> **难度级别**: L5 专家
> **预估学习时间**: 15-20 小时

---

## 目录

- [编译器后端与代码生成](#编译器后端与代码生成)
  - [目录](#目录)
  - [1. 编译器后端架构](#1-编译器后端架构)
    - [1.1 后端流程](#11-后端流程)
    - [1.2 后端关键挑战](#12-后端关键挑战)
  - [2. 指令选择](#2-指令选择)
    - [2.1 树模式匹配](#21-树模式匹配)
    - [2.2 LLVM TableGen](#22-llvm-tablegen)
    - [2.3 SelectionDAG](#23-selectiondag)
  - [3. 寄存器分配](#3-寄存器分配)
    - [3.1 图着色算法](#31-图着色算法)
    - [3.2 线性扫描算法](#32-线性扫描算法)
    - [3.3 LLVM寄存器分配](#33-llvm寄存器分配)
  - [4. 指令调度](#4-指令调度)
    - [4.1 列表调度](#41-列表调度)
    - [4.2 软件流水线](#42-软件流水线)
  - [5. 机器码生成](#5-机器码生成)
    - [5.1 汇编输出](#51-汇编输出)
    - [5.2 目标文件格式](#52-目标文件格式)
  - [关联导航](#关联导航)
    - [前置知识](#前置知识)
    - [后续延伸](#后续延伸)
    - [参考](#参考)

## 1. 编译器后端架构

### 1.1 后端流程

```
┌─────────────────────────────────────────────────────────────┐
│                    编译器后端流程                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  IR (SSA形式)                                               │
│     │                                                       │
│     ▼                                                       │
│  ┌─────────────────┐                                        │
│  │  指令选择        │  将IR指令映射到目标机器指令             │
│  │  (Instruction   │  模式匹配、动态规划、图覆盖              │
│  │   Selection)    │                                        │
│  └────────┬────────┘                                        │
│           ▼                                                 │
│  ┌─────────────────┐                                        │
│  │  寄存器分配      │  将虚拟寄存器分配到物理寄存器/内存      │
│  │  (Register      │  图着色、线性扫描、区间分析              │
│  │   Allocation)   │                                        │
│  └────────┬────────┘                                        │
│           ▼                                                 │
│  ┌─────────────────┐                                        │
│  │  指令调度        │  重排指令顺序优化性能                   │
│  │  (Instruction   │  列表调度、轨迹调度、软件流水线          │
│  │   Scheduling)   │                                        │
│  └────────┬────────┘                                        │
│           ▼                                                 │
│  ┌─────────────────┐                                        │
│  │  汇编生成        │  输出目标汇编代码                       │
│  │  (Assembly      │  指令编码、伪指令展开                    │
│  │   Generation)   │                                        │
│  └────────┬────────┘                                        │
│           ▼                                                 │
│  目标汇编代码                                                │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 后端关键挑战

```
┌─────────────────────────────────────────────────────────────┐
│                      后端优化挑战                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. 指令选择                                                │
│     • 一个IR指令可能对应多个机器指令序列                     │
│     • 目标：最小化指令数/执行周期/代码大小                   │
│     • 方法：树模式匹配、DAG覆盖、ILP优化                     │
│                                                             │
│  2. 寄存器分配                                              │
│     • 物理寄存器数量有限                                     │
│     • 目标：最小化内存访问（ spill / reload）               │
│     • 问题：图着色是NP完全                                   │
│                                                             │
│  3. 指令调度                                                │
│     • 利用指令级并行(ILP)                                    │
│     • 避免流水线停顿                                         │
│     • 保持数据依赖                                           │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 2. 指令选择

### 2.1 树模式匹配

```
IR表达式树：
        add
       /   \
     load   mul
      /     /  \
    addr  reg  const:4

目标指令模式（x86-64）：
1. ADD r, r      →  add   %src, %dst
2. ADD r, imm    →  add   $imm, %dst
3. MUL r, r      →  imul  %src, %dst
4. MUL r, imm    →  imul  $imm, %dst
5. MOV r, m      →  mov   addr, %dst

匹配结果：
mov   addr, %rax      ; load → rax
imul  $4, %rbx        ; mul reg,4 → rbx
add   %rbx, %rax      ; add rax, rbx
```

### 2.2 LLVM TableGen

```tablegen
// LLVM指令描述的TableGen语法

// 定义寄存器类
def GPR32 : RegisterClass<"X86", [i32], 32,
  (add EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI,
       R8D, R9D, R10D, R11D, R12D, R13D, R14D, R15D)>;

// 定义指令
def ADD32rr : Instruction {
  let Namespace = "X86";
  dag OutOperandList = (outs GPR32:$dst);
  dag InOperandList = (ins GPR32:$src1, GPR32:$src2);
  let AsmString = "add{l}\t{$src2, $dst|$dst, $src2}";

  // 指令编码
  let Encoding = {
    // REX前缀（如果需要）
    // 0x01 /r: ADD r/m32, r32
    let Opcode = 0x01;
    let ModRM = { mod=3, reg=src2, rm=dst };
  };

  // 模式匹配
  let Pattern = [(set GPR32:$dst,
                  (add GPR32:$src1, GPR32:$src2))];
}

def ADD32ri : Instruction {
  let OutOperandList = (outs GPR32:$dst);
  let InOperandList = (ins GPR32:$src1, i32imm:$src2);
  let AsmString = "add{l}\t{$src2, $dst|$dst, $src2}";

  // 0x81 /0 id: ADD r/m32, imm32
  let Pattern = [(set GPR32:$dst,
                  (add GPR32:$src1, imm:$src2))];
}
```

### 2.3 SelectionDAG

```cpp
// LLVM SelectionDAG节点
enum ISD::NodeType {
  // 控制流
  BR,        // 无条件分支
  BRCOND,    // 条件分支
  BR_CC,     // 带条件码的分支

  // 算术
  ADD, SUB, MUL, SDIV, UDIV,
  FADD, FSUB, FMUL, FDIV,

  // 逻辑
  AND, OR, XOR, SHL, SRA, SRL,

  // 内存
  LOAD, STORE,

  // 类型转换
  TRUNCATE, ZERO_EXTEND, SIGN_EXTEND,
  FP_TO_SINT, SINT_TO_FP,

  // 其他
  CALL, RET, SELECT, SETCC
};

// 示例：a + b * c 的SelectionDAG
//
//     t4: i32 = add t1, t3
//         /     \
//     t1: i32  t3: i32 = mul t1, t2
//       (a)       /     \
//              t1: i32  t2: i32
//               (b)      (c)
```

---

## 3. 寄存器分配

### 3.1 图着色算法

```
寄存器干扰图构建：

代码：
a = load x
b = a + 1
c = a * 2
d = b + c

活跃区间：
a: [1, 3]  (在d=b+c前一直活跃)
b: [2, 4]
c: [3, 4]
d: [4, 5]

干扰图：
    a ────┬──── b
          │
          └──── c ──── d

    a与b冲突（同时活跃）
    a与c冲突
    b与c冲突
    b与d冲突
    c与d冲突

图着色（3个寄存器：R0, R1, R2）：
• a → R0
• b → R1
• c → R2
• d → R0 (d与b,c冲突，但a已死，可重用R0)
```

### 3.2 线性扫描算法

```cpp
// 伪代码：线性扫描寄存器分配

void LinearScanRegisterAllocation() {
    // 1. 计算活跃区间
    Set<Interval> active = {};
    List<Interval> intervals = ComputeLiveIntervals();
    Sort(intervals by start point);

    for each interval i in intervals:
        // 2. 过期区间处理
        ExpireOldIntervals(i);

        // 3. 检查是否有空闲寄存器
        if active.size() == R:
            SpillAtInterval(i);
        else:
            // 4. 分配寄存器
            i.register = GetFreeRegister();
            active.add(i);
    }
}

void ExpireOldIntervals(Interval i) {
    for each interval j in active sorted by end point:
        if j.end >= i.start:
            return;
        active.remove(j);
        FreeRegister(j.register);
}

void SpillAtInterval(Interval i) {
    // 选择spill候选：最后结束的区间
    spill = active.last();  // 最大end point

    if spill.end > i.end:
        // Spill后面的区间，重用其寄存器
        i.register = spill.register;
        spill.spillSlot = AllocateStackSlot();
        active.remove(spill);
        active.add(i);
    else:
        // Spill当前区间
        i.spillSlot = AllocateStackSlot();
}
```

### 3.3 LLVM寄存器分配

```cpp
// LLVM寄存器分配流程

// 1. 活跃变量分析
LiveIntervals LIS;
LIS.compute(fn);

// 2. 寄存器分配
// 选择算法：
// - RegAllocBasic: 基础线性扫描
// - RegAllocGreedy: 改进的贪婪算法（默认）
// - RegAllocPBQP: PBQP求解器

// 贪婪算法主要步骤：
// a) 分割活跃区间
// b) 全局分配（优先级高的先分配）
// c) 本地分配（处理被分割的区间）
// d) 处理spill（插入load/store）

// 3. 虚拟寄存器重写
VirtRegMap VRM;
VRM.rewrite(fn);
```

---

## 4. 指令调度

### 4.1 列表调度

```
依赖图：
       [1] load a
          │
          ▼
       [2] load b
          │
          ▼
       [3] mul  ; a * b
          │
          ▼
       [4] load c
          │
          ▼
       [5] add  ; + c
          │
          ▼
       [6] store

延迟（周期）：
load: 3, mul: 2, add: 1, store: 1

列表调度过程：
周期1: 可调度: {1, 2} → 调度 1 (load a)
周期2: 可调度: {2} → 调度 2 (load b)
周期3: 可调度: {} (等待load完成)
周期4: 可调度: {}
周期5: 可调度: {3} → 调度 3 (mul), 可调度: {4} → 调度 4 (load c)
周期6: 可调度: {}
周期7: 可调度: {5} → 调度 5 (add)
周期8: 可调度: {6} → 调度 6 (store)

总周期: 8
```

### 4.2 软件流水线

```
循环展开示例：
原始循环：
for (i = 0; i < N; i++) {
    a[i] = b[i] * c[i];
}

展开4次：
for (i = 0; i < N; i += 4) {
    a[i]   = b[i]   * c[i];
    a[i+1] = b[i+1] * c[i+1];
    a[i+2] = b[i+2] * c[i+2];
    a[i+3] = b[i+3] * c[i+3];
}

软件流水线（模调度）：
// 假设：load延迟2周期，mul延迟3周期
// II (Initiation Interval) = 1

周期  1    2    3    4    5    6    7
     load ──→ load ──→ mul ──→ mul ──→ store
     load ──→ load ──→ mul ──→ mul ──→ store
     load ──→ load ──→ mul ──→ mul ──→ store
     ...

Prolog:
load b[0], c[0]
load b[1], c[1]

Kernel (重复N-2次):
mul b[0], c[0] → t0
load b[2], c[2]
store t0 → a[0]
mul b[1], c[1] → t1
load b[3], c[3]
store t1 → a[1]

Epilog:
mul b[N-2], c[N-2]
store → a[N-2]
mul b[N-1], c[N-1]
store → a[N-1]
```

---

## 5. 机器码生成

### 5.1 汇编输出

```cpp
// LLVM汇编打印

// 函数定义
void X86AsmPrinter::emitFunctionHeader() {
    // 输出函数标签
    OutStreamer->emitLabel(CurrentFnSym);

    // 输出函数开始指令
    emitFunctionBody();
}

// 指令编码
void X86AsmPrinter::emitInstruction(const MachineInstr *MI) {
    // 获取指令编码
    uint64_t Bits = getBinaryCodeForInstr(MI);

    // 处理操作数
    for (const MachineOperand &MO : MI->operands()) {
        if (MO.isReg()) {
            // 寄存器操作数
            encodeRegister(MO.getReg());
        } else if (MO.isImm()) {
            // 立即数操作数
            encodeImmediate(MO.getImm());
        } else if (MO.isGlobal()) {
            // 全局符号
            encodeRelocation(MO.getGlobal());
        }
    }

    // 输出编码后的字节
    emitBytes(Bits);
}
```

### 5.2 目标文件格式

```
ELF文件结构（简化）：

┌─────────────────────────────────────┐
│            ELF Header               │
│  - 魔数 (0x7F 'E' 'L' 'F')          │
│  - 架构 (32/64位)                   │
│  - 字节序                           │
│  - 节表偏移                         │
└─────────────────────────────────────┘
│            Program Headers          │
│  - 加载段信息                       │
└─────────────────────────────────────┘
│            .text Section            │
│  - 机器码                           │
└─────────────────────────────────────┘
│            .data Section            │
│  - 初始化数据                       │
└─────────────────────────────────────┘
│            .rodata Section          │
│  - 只读数据（字符串、常量）          │
└─────────────────────────────────────┘
│            .bss Section             │
│  - 未初始化数据（仅大小）            │
└─────────────────────────────────────┘
│            .symtab Section          │
│  - 符号表（函数、变量）              │
└─────────────────────────────────────┘
│            .rel.text Section        │
│  - 重定位信息                       │
└─────────────────────────────────────┘
│            Section Headers          │
│  - 节描述符数组                     │
└─────────────────────────────────────┘
```

---

## 关联导航

### 前置知识

- [AST与IR](./01_AST_and_IR.md)
- [计算机体系结构](../09_Physical_Machine_Layer/README.md)

### 后续延伸

- [优化技术](./03_Optimization_Techniques.md)
- [链接与加载](../08_Linking_Loading_Topology/README.md)

### 参考

- Engineering a Compiler, Cooper & Torczon
- Advanced Compiler Design and Implementation, Muchnick
- LLVM Documentation: Code Generation
