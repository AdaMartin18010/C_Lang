# 栈式虚拟机实现

> **层级定位**: 23 VM Implementation / 基础层
> **前置知识**: [C到汇编：控制流](../22_Assembler_Implementation/04_C_to_Assembly_Control.md)
> **后续延伸**: [C子集解释器](./02_C_Subset_Interpreter.md)
> **横向关联**: [Nand2Tetris VM规范](https://www.nand2tetris.org/project07), [现有VM内容](../03_System_Technology_Domains/01_Virtual_Machine_Interpreter/README.md)
> **预计学习时间**: 2周
> **实践验证**: [stack_vm.c](../../examples/vm/stack_vm.c)

---

## 核心洞察

```
栈式虚拟机是理解编译和执行的最简洁模型。
它抽象了硬件细节，专注于计算的本质：

1. 操作数在栈上传递
2. 指令操作栈顶元素
3. 没有寄存器的复杂性
4. 易于实现和验证

通过实现栈式VM，您将理解：
- 字节码的设计原则
- 栈帧的管理
- 从高级语言到字节码的翻译
- 虚拟机和物理机的对应关系

这是从汇编到高级语言的中间层抽象。
```

---

## 1. 栈式虚拟机架构

### 1.1 基本组件

```
┌─────────────────────────────────────────────────────────────┐
│                    栈式虚拟机架构                             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐  │
│  │   代码段     │    │   数据段     │    │   调用栈     │  │
│  │  (字节码)    │    │  (全局变量)  │    │  (操作数)    │  │
│  └──────┬───────┘    └──────────────┘    └──────┬───────┘  │
│         │                                       │          │
│         │         ┌──────────────┐              │          │
│         └────────►│    执行引擎   │◄─────────────┘          │
│                   │              │                         │
│                   │  • 指令译码   │                         │
│                   │  • 栈操作     │                         │
│                   │  • 算术逻辑   │                         │
│                   └──────┬───────┘                         │
│                          │                                  │
│                   ┌──────▼───────┐                         │
│                   │  程序计数器  │                         │
│                   │    (PC)      │                         │
│                   └──────────────┘                         │
│                                                             │
└─────────────────────────────────────────────────────────────┘

核心思想：
- 所有计算通过操作栈进行
- 指令从代码段取出，操作栈，更新PC
- 没有寄存器，只有栈顶和次栈顶
```

### 1.2 指令集设计

```
栈式VM指令分类:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

1. 栈操作指令
   ┌─────────────────────────────────────────────┐
   │ PUSH n    │ 将常数n压栈                      │
   │ POP       │ 弹出栈顶                         │
   │ DUP       │ 复制栈顶                         │
   │ SWAP      │ 交换栈顶和次栈顶                  │
   └─────────────────────────────────────────────┘

2. 算术指令
   ┌─────────────────────────────────────────────┐
   │ ADD       │ 弹出两个值，压入和                │
   │ SUB       │ 弹出两个值，压入差                │
   │ MUL       │ 弹出两个值，压入积                │
   │ DIV       │ 弹出两个值，压入商                │
   │ NEG       │ 栈顶取反                         │
   └─────────────────────────────────────────────┘

3. 逻辑指令
   ┌─────────────────────────────────────────────┐
   │ AND       │ 按位与                           │
   │ OR        │ 按位或                           │
   │ NOT       │ 按位非                           │
   │ EQ        │ 等于比较，压入0或1                │
   │ LT        │ 小于比较                         │
   │ GT        │ 大于比较                         │
   └─────────────────────────────────────────────┘

4. 控制流指令
   ┌─────────────────────────────────────────────┐
   │ JUMP addr │ 无条件跳转到addr                  │
   │ JZ  addr  │ 栈顶为0则跳转                     │
   │ JNZ addr  │ 栈顶非0则跳转                     │
   │ CALL addr │ 调用函数                         │
   │ RET       │ 返回                             │
   └─────────────────────────────────────────────┘

5. 内存指令
   ┌─────────────────────────────────────────────┐
   │ LOAD  idx │ 从局部变量idx加载到栈             │
   │ STORE idx │ 存储栈顶到局部变量idx             │
   │ LOADG idx │ 从全局变量idx加载                 │
   │ STOREG idx│ 存储到全局变量idx                 │
   └─────────────────────────────────────────────┘

6. I/O指令
   ┌─────────────────────────────────────────────┐
   │ PRINT     │ 打印栈顶                         │
   │ READ      │ 读取输入压栈                      │
   └─────────────────────────────────────────────┘
```

### 1.3 指令编码

```c
// 指令枚举（操作码）
typedef enum {
    OP_NOP = 0,

    // 栈操作
    OP_PUSH,        // 后跟1字节操作数
    OP_POP,
    OP_DUP,
    OP_SWAP,

    // 算术
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_NEG,
    OP_MOD,

    // 逻辑
    OP_AND,
    OP_OR,
    OP_NOT,
    OP_EQ,
    OP_LT,
    OP_GT,

    // 控制流
    OP_JUMP,        // 后跟2字节地址
    OP_JZ,          // 条件跳转
    OP_JNZ,         // 条件跳转
    OP_CALL,        // 函数调用
    OP_RET,         // 返回

    // 内存
    OP_LOAD,        // 局部变量
    OP_STORE,
    OP_LOADG,       // 全局变量
    OP_STOREG,

    // I/O
    OP_PRINT,
    OP_READ,

    OP_HALT         // 停止执行
} Opcode;

// 指令格式
// 0-127: 单字节指令（无操作数）
// 128-191: 单字节操作数
// 192-255: 双字节操作数

// 示例字节码
uint8_t program[] = {
    OP_PUSH, 5,         // 压入5
    OP_PUSH, 3,         // 压入3
    OP_ADD,             // 相加，栈顶现在是8
    OP_PRINT,           // 打印8
    OP_HALT
};
```

---

## 2. C语言实现

### 2.1 基础VM实现

```c
// stack_vm.c - 简化栈式虚拟机

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define STACK_SIZE 256
#define CODE_SIZE 1024
#define GLOBALS_SIZE 256

// ============ VM结构 ============
typedef struct {
    // 执行状态
    uint8_t *code;          // 字节码
    int code_size;
    int pc;                 // 程序计数器

    // 操作数栈
    int stack[STACK_SIZE];
    int sp;                 // 栈指针（指向下一个空闲位置）

    // 全局变量
    int globals[GLOBALS_SIZE];

    // 调用栈（用于函数调用）
    int call_stack[256];
    int csp;                // 调用栈指针

    // 局部变量（简化：假设单一层级）
    int locals[256];
    int fp;                 // 帧指针

    bool running;
} VM;

// ============ VM操作 ============
void vm_init(VM *vm, uint8_t *code, int code_size) {
    vm->code = code;
    vm->code_size = code_size;
    vm->pc = 0;
    vm->sp = 0;
    vm->csp = 0;
    vm->fp = 0;
    vm->running = true;

    for (int i = 0; i < GLOBALS_SIZE; i++) vm->globals[i] = 0;
    for (int i = 0; i < 256; i++) vm->locals[i] = 0;
}

// 栈操作辅助函数
static inline void push(VM *vm, int value) {
    if (vm->sp >= STACK_SIZE) {
        fprintf(stderr, "Stack overflow!\n");
        exit(1);
    }
    vm->stack[vm->sp++] = value;
}

static inline int pop(VM *vm) {
    if (vm->sp <= 0) {
        fprintf(stderr, "Stack underflow!\n");
        exit(1);
    }
    return vm->stack[--vm->sp];
}

static inline int peek(VM *vm) {
    if (vm->sp <= 0) {
        fprintf(stderr, "Stack empty!\n");
        exit(1);
    }
    return vm->stack[vm->sp - 1];
}

// ============ 执行循环 ============
void vm_run(VM *vm) {
    while (vm->running && vm->pc < vm->code_size) {
        uint8_t opcode = vm->code[vm->pc++];

        switch (opcode) {
            // 栈操作
            case OP_PUSH: {
                int value = vm->code[vm->pc++];
                push(vm, value);
                break;
            }
            case OP_POP:
                pop(vm);
                break;
            case OP_DUP: {
                int v = peek(vm);
                push(vm, v);
                break;
            }
            case OP_SWAP: {
                int a = pop(vm);
                int b = pop(vm);
                push(vm, a);
                push(vm, b);
                break;
            }

            // 算术
            case OP_ADD: {
                int b = pop(vm);
                int a = pop(vm);
                push(vm, a + b);
                break;
            }
            case OP_SUB: {
                int b = pop(vm);
                int a = pop(vm);
                push(vm, a - b);
                break;
            }
            case OP_MUL: {
                int b = pop(vm);
                int a = pop(vm);
                push(vm, a * b);
                break;
            }
            case OP_DIV: {
                int b = pop(vm);
                int a = pop(vm);
                push(vm, a / b);
                break;
            }
            case OP_NEG:
                push(vm, -pop(vm));
                break;

            // 逻辑
            case OP_EQ: {
                int b = pop(vm);
                int a = pop(vm);
                push(vm, a == b ? 1 : 0);
                break;
            }
            case OP_LT: {
                int b = pop(vm);
                int a = pop(vm);
                push(vm, a < b ? 1 : 0);
                break;
            }
            case OP_GT: {
                int b = pop(vm);
                int a = pop(vm);
                push(vm, a > b ? 1 : 0);
                break;
            }

            // 控制流
            case OP_JUMP: {
                int addr = (vm->code[vm->pc] << 8) | vm->code[vm->pc + 1];
                vm->pc = addr;
                break;
            }
            case OP_JZ: {
                int addr = (vm->code[vm->pc] << 8) | vm->code[vm->pc + 1];
                vm->pc += 2;
                if (pop(vm) == 0) vm->pc = addr;
                break;
            }
            case OP_JNZ: {
                int addr = (vm->code[vm->pc] << 8) | vm->code[vm->pc + 1];
                vm->pc += 2;
                if (pop(vm) != 0) vm->pc = addr;
                break;
            }

            // 内存
            case OP_LOAD: {
                int idx = vm->code[vm->pc++];
                push(vm, vm->locals[vm->fp + idx]);
                break;
            }
            case OP_STORE: {
                int idx = vm->code[vm->pc++];
                vm->locals[vm->fp + idx] = pop(vm);
                break;
            }
            case OP_LOADG: {
                int idx = vm->code[vm->pc++];
                push(vm, vm->globals[idx]);
                break;
            }
            case OP_STOREG: {
                int idx = vm->code[vm->pc++];
                vm->globals[idx] = pop(vm);
                break;
            }

            // I/O
            case OP_PRINT:
                printf("%d\n", pop(vm));
                break;
            case OP_READ: {
                int value;
                scanf("%d", &value);
                push(vm, value);
                break;
            }

            case OP_HALT:
                vm->running = false;
                break;

            default:
                fprintf(stderr, "Unknown opcode: %d\n", opcode);
                vm->running = false;
        }
    }
}
```

### 2.2 函数调用实现

```c
// 扩展：函数调用支持

// 函数调用指令
void op_call(VM *vm) {
    // 读取目标地址
    int addr = (vm->code[vm->pc] << 8) | vm->code[vm->pc + 1];
    vm->pc += 2;

    // 保存返回地址
    vm->call_stack[vm->csp++] = vm->pc;

    // 保存旧的帧指针
    vm->call_stack[vm->csp++] = vm->fp;

    // 设置新的帧指针
    vm->fp = vm->sp;

    // 跳转到函数
    vm->pc = addr;
}

void op_ret(VM *vm) {
    // 获取返回值（在栈顶）
    int retval = pop(vm);

    // 恢复栈指针（清除局部变量）
    vm->sp = vm->fp;

    // 恢复旧的帧指针
    vm->fp = vm->call_stack[--vm->csp];

    // 恢复返回地址
    vm->pc = vm->call_stack[--vm->csp];

    // 压入返回值
    push(vm, retval);
}

// 调用示例：计算阶乘
uint8_t factorial_code[] = {
    // factorial(n):
    // 参数n在locals[0]
    OP_LOAD, 0,         // 加载n
    OP_PUSH, 1,         // 压入1
    OP_EQ,              // n == 1?
    OP_JZ, 0, 12,       // 如果不等于，跳到递归情况 (地址12)

    // 基准情况：返回1
    OP_PUSH, 1,
    OP_RET,

    // 递归情况：返回 n * factorial(n-1)
    OP_LOAD, 0,         // 加载n
    OP_LOAD, 0,         // 加载n
    OP_PUSH, 1,
    OP_SUB,             // n - 1
    OP_CALL, 0, 0,      // 调用factorial (地址0)
    OP_MUL,             // n * result
    OP_RET
};
```

---

## 3. 从C到字节码的翻译

### 3.1 表达式翻译

```
表达式翻译规则:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

中缀表达式 → 后缀表达式（逆波兰表示法）→ 字节码

示例: (3 + 4) * 5

中缀:    (3 + 4) * 5
后缀:    3 4 + 5 *
字节码:
    PUSH 3
    PUSH 4
    ADD
    PUSH 5
    MUL

示例: a + b * c
后缀: a b c * +
字节码:
    LOAD 0    # a
    LOAD 1    # b
    LOAD 2    # c
    MUL
    ADD
```

```c
// 简单的表达式编译器
void compile_expression(const char *expr) {
    // 使用调度场算法转换为后缀表达式
    // 然后生成字节码

    // 简化示例：直接生成3+4的字节码
    uint8_t code[] = {
        OP_PUSH, 3,
        OP_PUSH, 4,
        OP_ADD,
        OP_PRINT,
        OP_HALT
    };

    VM vm;
    vm_init(&vm, code, sizeof(code));
    vm_run(&vm);
}
```

### 3.2 控制流翻译

```c
// C代码
if (a > b) {
    max = a;
} else {
    max = b;
}
```

```
字节码:
    LOAD 0        # a
    LOAD 1        # b
    GT            # a > b?
    JZ else       # 如果假，跳到else
    LOAD 0        # a
    STORE 2       # max = a
    JUMP end
else:
    LOAD 1        # b
    STORE 2       # max = b
end:
```

```c
// 编译器生成代码示例
typedef struct {
    uint8_t *code;
    int size;
    int capacity;
} CodeGen;

void emit(CodeGen *cg, uint8_t byte) {
    if (cg->size >= cg->capacity) {
        cg->capacity *= 2;
        cg->code = realloc(cg->code, cg->capacity);
    }
    cg->code[cg->size++] = byte;
}

void compile_if(CodeGen *cg, int cond_addr, int then_addr, int else_addr) {
    // 条件代码已经生成
    emit(cg, OP_JZ);
    int else_jump = cg->size;
    emit(cg, 0); emit(cg, 0);  // 占位符

    // then分支
    // ... 生成then代码
    emit(cg, OP_JUMP);
    int end_jump = cg->size;
    emit(cg, 0); emit(cg, 0);  // 占位符

    // 回填else地址
    cg->code[else_jump] = (cg->size >> 8) & 0xFF;
    cg->code[else_jump + 1] = cg->size & 0xFF;

    // else分支
    // ... 生成else代码

    // 回填end地址
    cg->code[end_jump] = (cg->size >> 8) & 0xFF;
    cg->code[end_jump + 1] = cg->size & 0xFF;
}
```

### 3.3 循环翻译

```c
// C代码
for (int i = 0; i < 10; i++) {
    sum += i;
}
```

```
字节码:
    PUSH 0
    STORE 0       # i = 0
loop:
    LOAD 0        # i
    PUSH 10
    LT            # i < 10?
    JZ end        # 如果不小于，结束

    LOAD 1        # sum
    LOAD 0        # i
    ADD
    STORE 1       # sum += i

    LOAD 0        # i
    PUSH 1
    ADD
    STORE 0       # i++

    JUMP loop     # 继续循环
end:
```

---

## 4. Nand2Tetris VM连接

### 4.1 Nand2Tetris VM概述

```
Nand2Tetris VM规范（Hack虚拟机）:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

与我们的简化VM的主要区别：
1. 基于段（segment）的内存模型
2. 8个虚拟寄存器（R0-R15）
3. 明确的内存段：local, argument, this, that, pointer, temp, static

内存段:
┌─────────────┬─────────────────────────────────┐
│ 段          │ 用途                            │
├─────────────┼─────────────────────────────────┤
│ local       │ 局部变量                        │
│ argument    │ 函数参数                        │
│ this        │ 当前对象的字段                  │
│ that        │ 数组访问                        │
│ pointer 0   │ 指向this                        │
│ pointer 1   │ 指向that                        │
│ temp        │ 临时变量（R5-R12）              │
│ static      │ 静态变量                        │
└─────────────┴─────────────────────────────────┘
```

### 4.2 Nand2Tetris VM指令

```
算术/逻辑指令:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

add/sub/neg/eq/gt/lt/and/or/not

所有操作都在栈上进行，没有显式操作数。

示例:
    push constant 7
    push constant 8
    add
    # 栈顶现在是15

内存访问指令:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

push segment index    # 将segment[index]压栈
pop segment index     # 弹出到segment[index]

示例:
    push local 0        # 压入locals[0]
    pop argument 1      # 存储到args[1]

函数调用指令:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

function name nVars   # 定义函数，nVars个局部变量
call name nArgs       # 调用函数，nArgs个参数
return                # 返回

示例:
    function mult 2     # mult函数，2个局部变量
    push argument 0
    push argument 1
    call mult 2         # 调用mult，2个参数
```

### 4.3 与物理机器的对比

```
从Nand2Tetris VM到物理机器:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

抽象层次:
┌─────────────────────────────────────────────┐
│ 高级语言 (Jack)                              │
│   ↓ 编译                                     │
├─────────────────────────────────────────────┤
│ VM字节码                                     │ ← 你在这里
│   ↓ VM翻译器                                 │
├─────────────────────────────────────────────┤
│ 汇编语言 (Hack)                              │
│   ↓ 汇编器                                   │
├─────────────────────────────────────────────┤
│ 机器码                                       │
│   ↓ 硬件执行                                 │
├─────────────────────────────────────────────┤
│ Hack计算机                                   │
│   • CPU (ALU + 寄存器)                       │
│   • 内存 (RAM + ROM)                         │
└─────────────────────────────────────────────┘

对应关系:
┌──────────────────┬──────────────────────────────┐
│ VM概念           │ 物理实现                     │
├──────────────────┼──────────────────────────────┤
│ 栈               │ RAM的一部分                  │
│ SP指针           │ 存储在RAM[0]                 │
│ 局部变量段       │ RAM中LCL指向的区域           │
│ 参数段           │ RAM中ARG指向的区域           │
│ 静态变量         │ RAM中特定地址                │
│ 函数调用         │ 保存返回地址，跳转           │
│ 返回             │ 恢复状态，跳转回             │
└──────────────────┴──────────────────────────────┘
```

---

## 5. VM到汇编的翻译

### 5.1 算术指令翻译

```
VM指令: add
汇编实现 (Hack):
    @SP
    AM=M-1      # SP--, A=M
    D=M         # D=*SP
    @SP
    AM=M-1      # SP--
    M=D+M       # *SP = D + *SP
    @SP
    M=M+1       # SP++

汇编实现 (RISC-V):
    lw   t0, 0(sp)      # 弹出第一个操作数
    addi sp, sp, 4
    lw   t1, 0(sp)      # 弹出第二个操作数
    add  t0, t1, t0     # 相加
    sw   t0, 0(sp)      # 压入结果（复用空间）
```

### 5.2 内存访问翻译

```
VM指令: push local 0
汇编实现 (RISC-V):
    lw   t0, -4(s0)     # 加载locals[0] (假设s0是帧指针)
    addi sp, sp, -4     # SP--
    sw   t0, 0(sp)      # *SP = value

VM指令: pop argument 1
汇编实现 (RISC-V):
    lw   t0, 0(sp)      # value = *SP
    addi sp, sp, 4      # SP++
    sw   t0, 4(a0)      # args[1] = value (假设a0指向参数区)
```

---

## 6. 学习检查点

### 6.1 自测问题

**基础理解：**

1. 栈式VM与寄存器式VM的主要区别是什么？
2. 为什么栈式VM的指令集通常比较简洁？
3. 解释PUSH 3, PUSH 4, ADD的执行过程和栈状态变化。

**进阶分析：**
4. 如何将中缀表达式`(a + b) * (c - d)`转换为栈式VM字节码？
5. 函数调用时，VM需要保存哪些状态？如何实现？
6. 比较Nand2Tetris VM与我们的简化VM的内存模型差异。

**动手实践：**
7. 为以下C代码生成字节码：

   ```c
   int factorial(int n) {
       if (n <= 1) return 1;
       return n * factorial(n - 1);
   }
   ```

1. 实现一个VM字节码到RISC-V汇编的翻译器。

### 6.2 答案与解析

<details>
<summary>点击查看答案</summary>

**问题1：**
栈式VM使用栈传递所有操作数，没有寄存器概念；寄存器式VM使用寄存器存储操作数，指令显式指定寄存器。栈式VM指令集更简洁，但通常执行效率较低。

**问题2：**
因为所有操作都通过栈进行，指令不需要显式指定操作数位置。例如ADD总是弹出栈顶两个值，不需要指定寄存器。

**问题3：**

```
执行前: []
PUSH 3: [3]
PUSH 4: [3, 4]
ADD:    [7]  (弹出3和4，压入7)
```

**问题4：**

```
字节码:
    LOAD 0    # a
    LOAD 1    # b
    ADD
    LOAD 2    # c
    LOAD 3    # d
    SUB
    MUL
```

**问题5：**
需要保存：返回地址、帧指针、局部变量。通过调用栈实现，CALL时保存，RET时恢复。

**问题6：**
Nand2Tetris使用多段内存模型（local, argument等），简化VM使用单一局部变量数组。Nand2Tetris更贴近实际高级语言实现。

</details>

---

## 7. 与知识库的深层连接

### 8.1 纵向连接

```
物理层:
└── [RISC-V汇编器](../22_Assembler_Implementation/02_RISCV_Assembler.md)
    └── 汇编指令的编码和执行

中间层（虚拟机）:
└── 你在这里 → 字节码在软件模拟的栈上执行

上层抽象:
├── [C子集解释器](./02_C_Subset_Interpreter.md)
│   └── 直接从语法树求值，或生成VM字节码
│
└── [编译器基础](../01_Core_Knowledge_System/15_Compiler_Basics/)
    └── 从源代码到目标代码的完整流程
```

### 8.2 横向连接

| 相关文档 | 连接点 |
|:---------|:-------|
| [现有VM内容](../03_System_Technology_Domains/01_Virtual_Machine_Interpreter/README.md) | 不同类型的虚拟机对比 |
| [JIT编译](../03_System_Technology_Domains/01_Virtual_Machine_Interpreter/03_JIT_Compilation.md) | VM字节码的动态编译 |
| [形式语义](../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/01_Operational_Semantics.md) | VM操作的操作语义描述 |

---

## 8. 延伸阅读

### 8.1 推荐资源

- **Nand2Tetris课程**: Project 7-8 (VM实现)
- **Crafting Interpreters**: 第2部分 - 字节码虚拟机
- **Lua 5.0实现论文**: 注册表式VM设计

### 8.2 实践项目

1. 扩展VM：添加字符串和数组支持
2. 实现一个简单的字节码编译器
3. 为VM添加垃圾回收
4. 实现VM字节码的JIT编译

---

*文档创建: 2026-03-29*
*上次更新: 2026-03-29*
*状态: 核心框架完成*
