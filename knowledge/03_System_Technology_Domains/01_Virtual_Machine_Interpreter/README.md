# 虚拟机与解释器 (Virtual Machine & Interpreter)

## 目录

- [虚拟机与解释器 (Virtual Machine \& Interpreter)](#虚拟机与解释器-virtual-machine--interpreter)
  - [目录](#目录)
  - [概述](#概述)
    - [虚拟机分类](#虚拟机分类)
    - [解释器 vs 编译器 vs 虚拟机](#解释器-vs-编译器-vs-虚拟机)
  - [字节码虚拟机架构](#字节码虚拟机架构)
    - [核心组件](#核心组件)
    - [简单字节码虚拟机实现](#简单字节码虚拟机实现)
  - [栈式与寄存器式VM](#栈式与寄存器式vm)
    - [架构对比](#架构对比)
    - [寄存器式VM实现示例](#寄存器式vm实现示例)
  - [垃圾回收](#垃圾回收)
    - [GC算法对比](#gc算法对比)
    - [简单GC实现示例](#简单gc实现示例)
  - [JIT编译基础](#jit编译基础)
    - [JIT编译流程](#jit编译流程)
    - [JIT优化技术](#jit优化技术)
  - [实践项目](#实践项目)
    - [项目：实现一个简单的栈式语言](#项目实现一个简单的栈式语言)
  - [参考资源](#参考资源)
    - [经典虚拟机实现](#经典虚拟机实现)
    - [推荐书籍](#推荐书籍)

---

## 概述

虚拟机（Virtual Machine, VM）是一种软件实现的计算机系统，它可以在物理硬件之上运行程序，提供跨平台兼容性和额外的安全隔离。解释器（Interpreter）则直接执行源代码或中间代码，无需事先编译为目标机器码。

### 虚拟机分类

```text
┌─────────────────────────────────────────────────────────────────────┐
│                        虚拟机分类体系                                │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │               系统虚拟机 (System VM)                         │   │
│  │  模拟完整硬件系统，运行完整操作系统                            │   │
│  │                                                             │   │
│  │  • VMware Workstation/ESXi                                   │   │
│  │  • VirtualBox                                                │   │
│  │  • Hyper-V / Xen / KVM                                       │   │
│  │  • 提供硬件虚拟化（CPU/内存/设备）                           │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │              进程虚拟机 (Process VM)                         │   │
│  │  运行在操作系统之上，执行特定类型的程序                        │   │
│  │                                                             │   │
│  │  ┌─────────────────────┐  ┌─────────────────────┐          │   │
│  │  │    语言运行时        │  │    字节码虚拟机      │          │   │
│  │  │                     │  │                     │          │   │
│  │  │  • JVM (Java)       │  │  • Python VM        │          │   │
│  │  │  • CLR (.NET)       │  │  • Lua VM           │          │   │
│  │  │  • Erlang BEAM      │  │  • Ruby YARV        │          │   │
│  │  │  • JavaScript V8    │  │  • WebAssembly      │          │   │
│  │  └─────────────────────┘  └─────────────────────┘          │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### 解释器 vs 编译器 vs 虚拟机

```text
┌─────────────────────────────────────────────────────────────────────┐
│                    执行模型对比                                      │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  【纯解释器模式】                                                    │
│  源码 → [词法分析] → [语法分析] → [解释执行] → 结果                │
│         Lexer       Parser      Interpreter                         │
│                                                                     │
│  特点: 启动快，执行慢，便于调试                                      │
│  代表: 早期 BASIC, shell脚本                                        │
│                                                                     │
│  【预编译字节码模式】                                                │
│  源码 → [编译] → 字节码 → [VM执行] → 结果                          │
│                Bytecode   Stack/Register VM                         │
│                                                                     │
│  特点: 平衡性能与可移植性                                            │
│  代表: Java, Python, Lua                                            │
│                                                                     │
│  【JIT编译模式】                                                     │
│  源码 → [编译] → 字节码 → [VM] → [JIT编译] → 机器码 → 执行        │
│                                        ↓                            │
│                                    热点检测                         │
│                                                                     │
│  特点: 运行时优化，接近原生性能                                      │
│  代表: Java HotSpot, V8, PyPy                                       │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 字节码虚拟机架构

### 核心组件

```text
┌─────────────────────────────────────────────────────────────────────────┐
│                    字节码虚拟机核心架构                                  │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                      指令解码器 (Decoder)                        │   │
│  │                                                                 │   │
│  │  字节码流 → [取指] → [解码] → [分发] → 执行单元                 │   │
│  │               Fetch   Decode   Dispatch                         │   │
│  │                                                                 │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────┐  ┌─────────────────────────────────────┐  │
│  │      执行引擎            │  │           内存管理器                │  │
│  │                         │  │                                     │  │
│  │  ┌─────────────────┐   │  │  ┌─────────────┐  ┌─────────────┐   │  │
│  │  │   指令实现表    │   │  │  │  堆 (Heap)  │  │ 栈 (Stack)  │   │  │
│  │  │  OP_ADD → impl  │   │  │  │             │  │             │   │  │
│  │  │  OP_SUB → impl  │   │  │  │ • 对象分配  │  │ • 局部变量  │   │  │
│  │  │  OP_CALL → impl │   │  │  │ • 垃圾回收  │  │ • 操作数栈  │   │  │
│  │  └─────────────────┘   │  │  └─────────────┘  └─────────────┘   │  │
│  │                         │  │                                     │  │
│  │  ┌─────────────────┐   │  │  ┌─────────────────────────────┐    │  │
│  │  │   调度循环      │   │  │  │        常量池                │    │  │
│  │  │  while(running) │   │  │  │                             │    │  │
│  │  │    op = fetch() │   │  │  │ • 字符串常量                │    │  │
│  │  │    decode(op)   │   │  │  │ • 数值常量                  │    │  │
│  │  └─────────────────┘   │  │  └─────────────────────────────┘    │  │
│  └─────────────────────────┘  └─────────────────────────────────────┘  │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 简单字节码虚拟机实现

```c
/**
 * @file simple_vm.c
 * @brief 简单栈式字节码虚拟机实现
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* 操作码定义 */
typedef enum {
    OP_NOP = 0,     /* 无操作 */
    OP_CONST,       /* 加载常量到栈 */
    OP_ADD,         /* 加法: 弹出两个值，压入和 */
    OP_SUB,         /* 减法 */
    OP_MUL,         /* 乘法 */
    OP_DIV,         /* 除法 */
    OP_NEG,         /* 取负 */
    OP_PRINT,       /* 打印栈顶 */
    OP_HALT         /* 停止 */
} OpCode;

/* 虚拟机状态 */
typedef struct {
    uint8_t *code;      /* 字节码 */
    int code_size;      /* 代码大小 */
    int pc;             /* 程序计数器 */

    int *stack;         /* 操作数栈 */
    int sp;             /* 栈指针 */
    int stack_size;     /* 栈大小 */

    int *constants;     /* 常量池 */
    int constant_count;
} VM;

/* 创建虚拟机 */
VM* vm_create(int stack_size, int max_constants) {
    VM *vm = malloc(sizeof(VM));
    vm->stack = malloc(sizeof(int) * stack_size);
    vm->stack_size = stack_size;
    vm->sp = 0;

    vm->constants = malloc(sizeof(int) * max_constants);
    vm->constant_count = 0;

    vm->pc = 0;
    return vm;
}

/* 压栈 */
void vm_push(VM *vm, int value) {
    if (vm->sp >= vm->stack_size) {
        fprintf(stderr, "Stack overflow!\n");
        exit(1);
    }
    vm->stack[vm->sp++] = value;
}

/* 弹栈 */
int vm_pop(VM *vm) {
    if (vm->sp <= 0) {
        fprintf(stderr, "Stack underflow!\n");
        exit(1);
    }
    return vm->stack[--vm->sp];
}

/* 执行字节码 */
void vm_run(VM *vm) {
    int a, b;

    while (vm->pc < vm->code_size) {
        uint8_t op = vm->code[vm->pc++];

        switch (op) {
            case OP_NOP:
                break;

            case OP_CONST: {
                /* 下一个字节是常量索引 */
                int const_idx = vm->code[vm->pc++];
                vm_push(vm, vm->constants[const_idx]);
                break;
            }

            case OP_ADD:
                b = vm_pop(vm);
                a = vm_pop(vm);
                vm_push(vm, a + b);
                break;

            case OP_SUB:
                b = vm_pop(vm);
                a = vm_pop(vm);
                vm_push(vm, a - b);
                break;

            case OP_MUL:
                b = vm_pop(vm);
                a = vm_pop(vm);
                vm_push(vm, a * b);
                break;

            case OP_DIV:
                b = vm_pop(vm);
                a = vm_pop(vm);
                if (b == 0) {
                    fprintf(stderr, "Division by zero!\n");
                    exit(1);
                }
                vm_push(vm, a / b);
                break;

            case OP_NEG:
                vm_push(vm, -vm_pop(vm));
                break;

            case OP_PRINT:
                printf("%d\n", vm_pop(vm));
                break;

            case OP_HALT:
                return;

            default:
                fprintf(stderr, "Unknown opcode: %d\n", op);
                exit(1);
        }
    }
}

/* 示例: 计算 (1 + 2) * 3 */
int main() {
    VM *vm = vm_create(256, 16);

    /* 设置常量池: [1, 2, 3] */
    vm->constants[0] = 1;
    vm->constants[1] = 2;
    vm->constants[2] = 3;
    vm->constant_count = 3;

    /* 字节码: (1 + 2) * 3 */
    uint8_t program[] = {
        OP_CONST, 0,    /* 加载常量0 (1) */
        OP_CONST, 1,    /* 加载常量1 (2) */
        OP_ADD,         /* 1 + 2 = 3 */
        OP_CONST, 2,    /* 加载常量2 (3) */
        OP_MUL,         /* 3 * 3 = 9 */
        OP_PRINT,       /* 打印结果 */
        OP_HALT
    };

    vm->code = program;
    vm->code_size = sizeof(program);

    printf("Running bytecode...\n");
    vm_run(vm);

    free(vm->stack);
    free(vm->constants);
    free(vm);

    return 0;
}
```

---

## 栈式与寄存器式VM

### 架构对比

```text
┌─────────────────────────────────────────────────────────────────────┐
│              栈式 VM vs 寄存器式 VM 对比                             │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  【栈式虚拟机】 (Stack-based)                                        │
│                                                                     │
│  代码: 1 + 2 * 3                                                    │
│                                                                     │
│  字节码:              栈状态:                                        │
│  CONST 1    ──→     [1]                                             │
│  CONST 2    ──→     [1, 2]                                          │
│  CONST 3    ──→     [1, 2, 3]                                       │
│  MUL        ──→     [1, 6]        (2*3=6)                          │
│  ADD        ──→     [7]           (1+6=7)                          │
│                                                                     │
│  特点:                                                              │
│  ✓ 指令编码紧凑 (无操作数或单操作数)                                 │
│  ✓ 实现简单，易于生成代码                                            │
│  ✗ 更多指令 (需显式的LOAD/STORE)                                    │
│  ✗ 更多内存访问                                                      │
│                                                                     │
│  代表: JVM, CLR, Python, Lua, WebAssembly                           │
│                                                                     │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  【寄存器式虚拟机】 (Register-based)                                 │
│                                                                     │
│  代码: 1 + 2 * 3                                                    │
│                                                                     │
│  字节码:              寄存器状态:                                    │
│  LOAD r0, #1                                          r0=1         │
│  LOAD r1, #2                                          r1=2         │
│  LOAD r2, #3                                          r2=3         │
│  MUL r3, r1, r2   ──→  r3 = r1 * r2 = 6              r3=6          │
│  ADD r0, r0, r3   ──→  r0 = r0 + r3 = 7              r0=7          │
│                                                                     │
│  特点:                                                              │
│  ✓ 更少指令 (操作隐式在寄存器)                                       │
│  ✓ 更少内存访问，执行更快                                            │
│  ✗ 指令编码较大 (需指定寄存器号)                                     │
│  ✗ 寄存器分配优化复杂                                                │
│                                                                     │
│  代表: Dalvik (Android), LuaJIT, QEMU                               │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### 寄存器式VM实现示例

```c
/**
 * @file register_vm.c
 * @brief 简单寄存器式虚拟机
 */

#include <stdio.h>
#include <stdint.h>

#define NUM_REGS 16

typedef enum {
    R_OP_NOP = 0,
    R_OP_LOAD,      /* LOAD r, imm: r = imm */
    R_OP_ADD,       /* ADD r1, r2, r3: r1 = r2 + r3 */
    R_OP_SUB,       /* SUB r1, r2, r3: r1 = r2 - r3 */
    R_OP_MUL,       /* MUL r1, r2, r3: r1 = r2 * r3 */
    R_OP_DIV,       /* DIV r1, r2, r3: r1 = r2 / r3 */
    R_OP_MOV,       /* MOV r1, r2: r1 = r2 */
    R_OP_PRINT,     /* PRINT r: print r */
    R_OP_HALT
} R_OpCode;

/* 指令格式: [opcode:8] [reg1:8] [reg2:8] [reg3:8] [imm:32] */
typedef struct {
    uint8_t opcode;
    uint8_t r1, r2, r3;
    int32_t imm;
} Instruction;

typedef struct {
    int32_t regs[NUM_REGS];
    Instruction *code;
    int pc;
    int code_size;
} R_VM;

void r_vm_run(R_VM *vm) {
    while (vm->pc < vm->code_size) {
        Instruction *instr = &vm->code[vm->pc++];

        switch (instr->opcode) {
            case R_OP_NOP:
                break;

            case R_OP_LOAD:
                vm->regs[instr->r1] = instr->imm;
                break;

            case R_OP_ADD:
                vm->regs[instr->r1] = vm->regs[instr->r2] + vm->regs[instr->r3];
                break;

            case R_OP_SUB:
                vm->regs[instr->r1] = vm->regs[instr->r2] - vm->regs[instr->r3];
                break;

            case R_OP_MUL:
                vm->regs[instr->r1] = vm->regs[instr->r2] * vm->regs[instr->r3];
                break;

            case R_OP_DIV:
                vm->regs[instr->r1] = vm->regs[instr->r2] / vm->regs[instr->r3];
                break;

            case R_OP_MOV:
                vm->regs[instr->r1] = vm->regs[instr->r2];
                break;

            case R_OP_PRINT:
                printf("R%d = %d\n", instr->r1, vm->regs[instr->r1]);
                break;

            case R_OP_HALT:
                return;
        }
    }
}

/* 计算: r0 = (10 + 20) * 3 */
int main() {
    Instruction program[] = {
        {R_OP_LOAD, 0, 0, 0, 10},   /* r0 = 10 */
        {R_OP_LOAD, 1, 0, 0, 20},   /* r1 = 20 */
        {R_OP_ADD, 0, 0, 1, 0},     /* r0 = r0 + r1 = 30 */
        {R_OP_LOAD, 2, 0, 0, 3},    /* r2 = 3 */
        {R_OP_MUL, 0, 0, 2, 0},     /* r0 = r0 * r2 = 90 */
        {R_OP_PRINT, 0, 0, 0, 0},   /* print r0 */
        {R_OP_HALT, 0, 0, 0, 0}
    };

    R_VM vm = {
        .regs = {0},
        .code = program,
        .pc = 0,
        .code_size = sizeof(program) / sizeof(Instruction)
    };

    printf("Register VM running...\n");
    r_vm_run(&vm);

    return 0;
}
```

---

## 垃圾回收

### GC算法对比

```text
┌─────────────────────────────────────────────────────────────────────┐
│                     垃圾回收算法对比                                 │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  【引用计数】 (Reference Counting)                                   │
│                                                                     │
│  原理: 每个对象维护引用计数，计数为0时回收                             │
│                                                                     │
│  优点:                                                              │
│  ✓ 实时回收，无停顿                                                 │
│  ✓ 实现简单                                                         │
│  ✓ 垃圾可被立即回收                                                 │
│                                                                     │
│  缺点:                                                              │
│  ✗ 无法处理循环引用                                                 │
│  ✗ 计数操作开销                                                     │
│  ✗ 需要原子操作保证线程安全                                          │
│                                                                     │
│  代表: Python(CPython), Swift, Objective-C                          │
│                                                                     │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  【标记-清除】 (Mark-Sweep)                                          │
│                                                                     │
│  原理: 从根对象出发标记可达对象，清除未标记对象                      │
│                                                                     │
│  阶段1-标记:                                                        │
│    ┌─────┐   ┌─────┐   ┌─────┐   ┌─────┐   ┌─────┐               │
│    │Root │ → │ A ✓ │ → │ B ✓ │   │ C ✗ │   │ D ✗ │               │
│    └─────┘   └─────┘   └─────┘   └─────┘   └─────┘               │
│                 ↓                                                   │
│              ┌─────┐                                                │
│              │ E ✓ │                                                │
│              └─────┘                                                │
│                                                                     │
│  阶段2-清除: 回收 C, D                                              │
│                                                                     │
│  代表: Ruby, Lua                                                    │
│                                                                     │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  【标记-整理】 (Mark-Compact)                                        │
│                                                                     │
│  原理: 标记存活对象，将其压缩到内存一端，消除碎片                    │
│                                                                     │
│  内存状态:          标记整理后:                                      │
│  ┌──┬──┬──┬──┬──┐   ┌──┬──┬──┬──┬──┐                              │
│  │A │  │B │  │C │   │A │B │C │  │  │                              │
│  └──┴──┴──┴──┴──┘   └──┴──┴──┴──┴──┘                              │
│    存活  空闲 存活                               存活    空闲       │
│                                                                     │
│  代表: .NET CLR, V8(老年代)                                         │
│                                                                     │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  【复制算法】 (Copying)                                              │
│                                                                     │
│  原理: 内存分两半，每次只使用一半，存活对象复制到另一半              │
│                                                                     │
│  From空间:          To空间(复制后):                                  │
│  ┌──┬──┬──┬──┬──┐   ┌──┬──┬──┬──┬──┐                              │
│  │A │B │  │C │  │   │A │B │C │  │  │                              │
│  └──┴──┴──┴──┴──┘   └──┴──┴──┴──┴──┘                              │
│  存活存活  存活                        无碎片                       │
│                                                                     │
│  代表: Java(新生代 Survivor区), V8(新生代)                          │
│                                                                     │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  【分代收集】 (Generational)                                         │
│                                                                     │
│  原理: 根据对象存活时间分代，不同代使用不同算法                      │
│                                                                     │
│  ┌─────────────────────────────────────────┐                        │
│  │              老年代 (Old)                │ ← 标记-清除/整理      │
│  │    存活时间长的对象，GC频率低           │   Full GC             │
│  ├─────────────────────────────────────────┤                        │
│  │      幸存区 (Survivor) S0  S1           │ ← 复制算法            │
│  ├─────────────────────────────────────────┤                        │
│  │              新生代 (Young)              │ ← 复制算法            │
│  │    新创建的对象，GC频率高，大部分死亡   │   Minor GC            │
│  └─────────────────────────────────────────┘                        │
│                                                                     │
│  假说: 大多数对象朝生夕死                                             │
│                                                                     │
│  代表: JVM HotSpot, .NET, V8                                        │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### 简单GC实现示例

```c
/**
 * @file simple_gc.c
 * @brief 简单标记-清除垃圾回收器
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define HEAP_SIZE 1024
#define MAX_ROOTS 64

/* 对象类型 */
typedef enum {
    OBJ_INT,
    OBJ_PAIR
} ObjectType;

/* 对象结构 */
typedef struct Object {
    ObjectType type;
    bool marked;
    struct Object *next;  /* 链表指针 */

    union {
        int value;              /* OBJ_INT */
        struct {                /* OBJ_PAIR */
            struct Object *head;
            struct Object *tail;
        } pair;
    } data;
} Object;

/* 堆管理器 */
typedef struct {
    Object *first_object;   /* 已分配对象链表 */
    Object *heap;           /* 堆内存 */
    int num_objects;
    int max_objects;        /* GC触发阈值 */

    Object *roots[MAX_ROOTS];
    int num_roots;
} VM;

/* 创建VM */
VM* vm_new(void) {
    VM *vm = malloc(sizeof(VM));
    vm->first_object = NULL;
    vm->heap = malloc(sizeof(Object) * HEAP_SIZE);
    vm->num_objects = 0;
    vm->max_objects = 8;  /* 初始阈值 */
    vm->num_roots = 0;
    return vm;
}

/* 添加根对象 */
void vm_add_root(VM *vm, Object *obj) {
    if (vm->num_roots >= MAX_ROOTS) {
        fprintf(stderr, "Too many roots!\n");
        exit(1);
    }
    vm->roots[vm->num_roots++] = obj;
}

/* 标记对象 */
void mark(Object *obj) {
    if (obj == NULL || obj->marked) return;

    obj->marked = true;

    if (obj->type == OBJ_PAIR) {
        mark(obj->data.pair.head);
        mark(obj->data.pair.tail);
    }
}

/* 标记阶段 */
void mark_all(VM *vm) {
    for (int i = 0; i < vm->num_roots; i++) {
        mark(vm->roots[i]);
    }
}

/* 清除阶段 */
void sweep(VM *vm) {
    Object **obj = &vm->first_object;
    while (*obj != NULL) {
        if (!(*obj)->marked) {
            /* 未标记，回收 */
            Object *unreached = *obj;
            *obj = unreached->next;
            vm->num_objects--;
            /* 实际内存池实现中会返回空闲列表 */
        } else {
            /* 清除标记 */
            (*obj)->marked = false;
            obj = &(*obj)->next;
        }
    }
}

/* 执行GC */
void gc(VM *vm) {
    printf("-- GC: %d objects before --\n", vm->num_objects);

    mark_all(vm);
    sweep(vm);

    printf("-- GC: %d objects after --\n", vm->num_objects);

    /* 调整阈值 */
    vm->max_objects = vm->num_objects * 2;
}

/* 分配对象 */
Object* new_object(VM *vm, ObjectType type) {
    if (vm->num_objects >= vm->max_objects) {
        gc(vm);
    }

    Object *obj = &vm->heap[vm->num_objects++];
    obj->type = type;
    obj->marked = false;
    obj->next = vm->first_object;
    vm->first_object = obj;

    return obj;
}

/* 创建整数 */
Object* new_int(VM *vm, int value) {
    Object *obj = new_object(vm, OBJ_INT);
    obj->data.value = value;
    return obj;
}

/* 创建数对 */
Object* new_pair(VM *vm, Object *head, Object *tail) {
    Object *obj = new_object(vm, OBJ_PAIR);
    obj->data.pair.head = head;
    obj->data.pair.tail = tail;
    return obj;
}

/* 打印对象 */
void print_object(Object *obj) {
    if (obj == NULL) {
        printf("null");
        return;
    }

    switch (obj->type) {
        case OBJ_INT:
            printf("%d", obj->data.value);
            break;
        case OBJ_PAIR:
            printf("(");
            print_object(obj->data.pair.head);
            printf(" . ");
            print_object(obj->data.pair.tail);
            printf(")");
            break;
    }
}

/* 示例程序 */
int main() {
    VM *vm = vm_new();

    printf("Creating objects...\n");

    /* 创建列表: (1 . (2 . (3 . null))) */
    Object *list = new_pair(vm,
        new_int(vm, 1),
        new_pair(vm,
            new_int(vm, 2),
            new_pair(vm,
                new_int(vm, 3),
                NULL)));

    vm_add_root(vm, list);

    printf("List: ");
    print_object(list);
    printf("\n\n");

    /* 创建一些临时对象 */
    printf("Creating temporary objects...\n");
    for (int i = 0; i < 10; i++) {
        Object *temp = new_int(vm, i);
        (void)temp; /* 临时对象，不加入根集合 */
    }

    printf("\nList after GC: ");
    print_object(list);
    printf("\n");

    free(vm->heap);
    free(vm);

    return 0;
}
```

---

## JIT编译基础

### JIT编译流程

```text
┌─────────────────────────────────────────────────────────────────────┐
│                    JIT 编译流程                                      │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐             │
│  │   字节码    │ →  │  热点检测   │ →  │  IR生成     │             │
│  │  解释执行   │    │  (计数器)   │    │  中间表示   │             │
│  └─────────────┘    └─────────────┘    └─────────────┘             │
│                                               │                     │
│                                               ↓                     │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐             │
│  │   执行      │ ←  │  机器码     │ ←  │  代码生成   │             │
│  │  机器码     │    │  缓存       │    │  后端       │             │
│  └─────────────┘    └─────────────┘    └─────────────┘             │
│                                                                     │
│  优化层次:                                                          │
│  ──────────────────────────────────────────────                     │
│  Level 0: 解释执行                                                  │
│  Level 1: 简单JIT (快速编译，基础优化)                              │
│  Level 2: 优化JIT (耗时编译，激进优化)                              │
│  Level 3: 去优化 (优化假设失败，回退)                               │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### JIT优化技术

```text
┌─────────────────────────────────────────────────────────────────────┐
│                    JIT 优化技术                                      │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  【内联缓存】 (Inline Caching)                                       │
│                                                                     │
│  原始代码:            优化后:                                        │
│  obj.method()    →   if (obj.type == CACHED_TYPE)                   │
│                         jump CACHED_METHOD                          │
│                      else                                           │
│                         slow_lookup()                               │
│                                                                     │
│  原理: 缓存方法查找结果，预测类型不变                                  │
│                                                                     │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  【隐藏类优化】 (Hidden Classes / Shape)                             │
│                                                                     │
│  对象结构:                                                          │
│  ┌──────────┐      ┌─────────┐                                     │
│  │ Hidden   │  →   │  Shape  │  {x: int, y: int}                   │
│  │ Class Ptr│      │  Table  │  offset(x)=0, offset(y)=8           │
│  ├──────────┤      └─────────┘                                     │
│  │  x = 10  │        ↑                                             │
│  │  y = 20  │        └────── 多个对象共享形状                        │
│  └──────────┘                                                       │
│                                                                     │
│  访问 obj.x: 直接通过 Shape 表获取偏移量，无需字典查找                 │
│                                                                     │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  【类型特化】 (Type Specialization)                                  │
│                                                                     │
│  通用加法:              特化后 (假设都是整数):                        │
│  add(a, b) {        add_int(a, b) {                                 │
│    if (is_int(a)      // 无类型检查                                  │
│        && is_int(b))  return a + b;  // 直接整数加法                 │
│      return a + b;   }                                              │
│    if (is_string(a))                                                │
│      return concat(a,b);                                            │
│    ...                                                              │
│  }                                                                  │
│                                                                     │
│  去优化: 假设失败时回退到通用代码                                      │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 实践项目

### 项目：实现一个简单的栈式语言

```c
/**
 * @file stacklang.c
 * @brief 简单栈式语言实现
 *
 * 语法示例:
 *   1 2 +           → 3
 *   5 3 - 4 *       → 8 (计算: (5-3)*4)
 *   : square dup * ;  (定义平方函数)
 *   5 square        → 25
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define STACK_SIZE 256
#define WORD_MAX 32
#define DICT_SIZE 256

/* 词类型 */
typedef enum {
    WORD_NUMBER,
    WORD_PRIMITIVE,
    WORD_USER
} WordType;

/* 原语函数指针 */
typedef void (*PrimitiveFunc)(void);

/* 词典条目 */
typedef struct Word {
    char name[WORD_MAX];
    WordType type;
    union {
        int value;              /* WORD_NUMBER */
        PrimitiveFunc prim;     /* WORD_PRIMITIVE */
        struct Word *code;      /* WORD_USER: 指向代码数组 */
    } data;
    struct Word *next;
} Word;

/* 虚拟机状态 */
typedef struct {
    int stack[STACK_SIZE];
    int sp;

    Word *dictionary;           /* 词典链表头 */
    Word dict_storage[DICT_SIZE];
    int dict_count;

    int compiling;              /* 编译模式标志 */
    Word *compile_buf[256];     /* 编译缓冲区 */
    int compile_len;
} ForthVM;

/* 创建VM */
ForthVM* vm_create(void) {
    ForthVM *vm = calloc(1, sizeof(ForthVM));
    return vm;
}

/* 栈操作 */
void push(ForthVM *vm, int n) {
    if (vm->sp >= STACK_SIZE) {
        fprintf(stderr, "Stack overflow\n");
        exit(1);
    }
    vm->stack[vm->sp++] = n;
}

int pop(ForthVM *vm) {
    if (vm->sp <= 0) {
        fprintf(stderr, "Stack underflow\n");
        exit(1);
    }
    return vm->stack[--vm->sp];
}

/* 查找词 */
Word* find_word(ForthVM *vm, const char *name) {
    for (Word *w = vm->dictionary; w != NULL; w = w->next) {
        if (strcmp(w->name, name) == 0) {
            return w;
        }
    }
    return NULL;
}

/* 添加原语 */
void add_primitive(ForthVM *vm, const char *name, PrimitiveFunc func) {
    Word *w = &vm->dict_storage[vm->dict_count++];
    strcpy(w->name, name);
    w->type = WORD_PRIMITIVE;
    w->data.prim = func;
    w->next = vm->dictionary;
    vm->dictionary = w;
}

/* 原语实现 */
void prim_add(ForthVM *vm) {
    int b = pop(vm);
    int a = pop(vm);
    push(vm, a + b);
}

void prim_sub(ForthVM *vm) {
    int b = pop(vm);
    int a = pop(vm);
    push(vm, a - b);
}

void prim_mul(ForthVM *vm) {
    int b = pop(vm);
    int a = pop(vm);
    push(vm, a * b);
}

void prim_dup(ForthVM *vm) {
    int a = pop(vm);
    push(vm, a);
    push(vm, a);
}

void prim_swap(ForthVM *vm) {
    int b = pop(vm);
    int a = pop(vm);
    push(vm, b);
    push(vm, a);
}

void prim_dot(ForthVM *vm) {
    printf("%d ", pop(vm));
}

void prim_emit(ForthVM *vm) {
    putchar(pop(vm));
}

void prim_cr(ForthVM *vm) {
    putchar('\n');
}

/* 执行词 */
void execute(ForthVM *vm, Word *w);

void execute(ForthVM *vm, Word *w) {
    switch (w->type) {
        case WORD_NUMBER:
            push(vm, w->data.value);
            break;
        case WORD_PRIMITIVE:
            w->data.prim(vm);
            break;
        case WORD_USER: {
            /* 执行用户定义词 */
            Word **code = w->data.code;
            while (*code != NULL) {
                execute(vm, *code);
                code++;
            }
            break;
        }
    }
}

/* 初始化词典 */
void init_dictionary(ForthVM *vm) {
    add_primitive(vm, "+", prim_add);
    add_primitive(vm, "-", prim_sub);
    add_primitive(vm, "*", prim_mul);
    add_primitive(vm, "dup", prim_dup);
    add_primitive(vm, "swap", prim_swap);
    add_primitive(vm, ".", prim_dot);
    add_primitive(vm, "emit", prim_emit);
    add_primitive(vm, "cr", prim_cr);
}

/* 解析并执行输入 */
void interpret(ForthVM *vm, const char *input) {
    char token[WORD_MAX];
    int pos = 0;

    while (sscanf(input + pos, "%31s", token) == 1) {
        pos += strlen(token);
        while (input[pos] == ' ') pos++;

        /* 检查是否是数字 */
        int num;
        if (sscanf(token, "%d", &num) == 1 && isdigit(token[0])) {
            if (vm->compiling) {
                /* 编译模式：添加到代码 */
                Word *w = &vm->dict_storage[vm->dict_count++];
                strcpy(w->name, "LIT");
                w->type = WORD_NUMBER;
                w->data.value = num;
                vm->compile_buf[vm->compile_len++] = w;
            } else {
                push(vm, num);
            }
            continue;
        }

        /* 查找词典 */
        Word *w = find_word(vm, token);
        if (w) {
            if (vm->compiling && w->type != WORD_PRIMITIVE) {
                /* 编译用户定义词 */
                vm->compile_buf[vm->compile_len++] = w;
            } else {
                execute(vm, w);
            }
        } else {
            fprintf(stderr, "Unknown word: %s\n", token);
        }
    }
}

/* 主函数 */
int main(void) {
    ForthVM *vm = vm_create();
    init_dictionary(vm);

    printf("StackLang 0.1 - Simple Stack-Based Language\n");
    printf("Examples: 1 2 + . cr    (prints 3 and newline)\n");
    printf("          5 3 - 4 * .   (prints 8)\n\n");

    char line[256];
    while (1) {
        printf("> ");
        if (!fgets(line, sizeof(line), stdin)) break;

        if (strncmp(line, "bye", 3) == 0) break;

        interpret(vm, line);
    }

    printf("\nGoodbye!\n");
    free(vm);
    return 0;
}
```

---

## 参考资源

### 经典虚拟机实现

| 项目 | 描述 | 链接 |
|------|------|------|
| Lua | 简洁高效的寄存器式VM | lua.org |
| Wren | 轻量级脚本语言VM | wren.io |
| Crafting Interpreters | 编译器/解释器教程 | craftinginterpreters.com |
| Writing a VM | 字节码VM教程 | github.com/DoctorWkt/acwj |

### 推荐书籍

- **《Crafting Interpreters》** - Robert Nystrom
- **《Virtual Machines》** - Iain D. Craig
- **《Garbage Collection》** - Jones & Lins

---

*最后更新: 2026年3月*:
