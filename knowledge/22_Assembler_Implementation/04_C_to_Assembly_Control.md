# C到汇编：控制流

> **层级定位**: 22 Assembler Implementation / 控制流映射层
> **前置知识**: [C到汇编：数据表示](./03_C_to_Assembly_Data.md), [RISC-V汇编器](./02_RISCV_Assembler.md)
> **后续延伸**: [栈式虚拟机](../23_VM_Implementation/01_Stack_Based_VM.md)
> **横向关联**: [函数与作用域](../01_Core_Knowledge_System/02_Core_Layer/04_Functions_Scope.md), [控制流](../01_Core_Knowledge_System/01_Basic_Layer/04_Control_Flow.md)
> **预计学习时间**: 2周
> **实践验证**: [c_to_asm_control.c](../../examples/assembler/c_to_asm_control.c)

---

## 核心洞察

```
C语言的控制流（if、for、while、switch、函数调用）
在汇编层面全部归结为：

1. 条件分支指令（beq, bne, blt, ...）
2. 无条件跳转（jal, jalr）
3. 比较操作（slt, slti）
4. 精心设计的标签和跳转目标

理解控制流的汇编映射，就是理解：
- "结构化编程"如何用"goto"实现
- 函数调用的栈帧约定
- 为什么递归需要栈
- 尾递归优化的可能性

这是从结构化抽象回到控制流图的旅程。
```

---

## 1. 条件分支的汇编实现

### 1.1 if-else 语句

```c
// C代码
if (a == b) {
    x = 1;
} else {
    x = 2;
}
```

```asm
# RISC-V汇编（基本版本）
    lw   t0, a
    lw   t1, b
    bne  t0, t1, else       # if (a != b) goto else
    li   t2, 1
    sw   t2, x              # x = 1
    j    endif              # goto endif
else:
    li   t2, 2
    sw   t2, x              # x = 2
endif:
    # 继续执行
```

```
控制流图:
    ┌─────────┐
    │  a == b │
    └────┬────┘
   真  /   \  假
      /     \
┌────▼───┐ ┌▼───────┐
│ x = 1  │ │ x = 2  │
└────┬───┘ └─┬──────┘
     \      /
      \    /
    ┌──▼────▼──┐
    │ 继续执行  │
    └──────────┘
```

### 1.2 复杂条件

```c
// C代码
if (a > 0 && b < 10) {
    x = 1;
}
```

```asm
# RISC-V汇编（短路求值）
    lw   t0, a
    ble  t0, zero, skip     # if (a <= 0) goto skip

    lw   t1, b
    li   t2, 10
    bge  t1, t2, skip       # if (b >= 10) goto skip

    li   t3, 1
    sw   t3, x              # x = 1
skip:
```

```
短路求值的汇编实现:
- 第一个条件为假时，直接跳过第二个条件的检查
- 这避免了潜在的副作用（如函数调用）被不必要地执行
```

### 1.3 条件表达式（三元运算符）

```c
// C代码
int max = (a > b) ? a : b;
```

```asm
# RISC-V汇编
    lw   t0, a
    lw   t1, b
    ble  t0, t1, else       # if (a <= b) goto else
    mv   t2, t0             # max = a
    j    end
else:
    mv   t2, t1             # max = b
end:
    sw   t2, max
```

---

## 2. 循环结构的汇编实现

### 2.1 while 循环

```c
// C代码
int i = 0;
while (i < 10) {
    sum += i;
    i++;
}
```

```asm
# RISC-V汇编（先检查条件）
    li   t0, 0              # i = 0
    li   t1, 0              # sum = 0

loop:
    li   t2, 10
    bge  t0, t2, end        # if (i >= 10) goto end

    add  t1, t1, t0         # sum += i
    addi t0, t0, 1          # i++

    j    loop               # goto loop
end:
    sw   t1, sum
    sw   t0, i
```

```
while循环结构:
    ┌────────────────┐
    │     i = 0      │
    └───────┬────────┘
            ▼
       ┌────────┐
  ┌────│ i < 10 │────┐
  │否   └────────┘   │是
  ▼                  ▼
┌───────┐      ┌──────────┐
│ 结束  │      │ sum += i │
└───────┘      │ i++      │
               └─────┬────┘
                     │
                     └──────→ (回到条件检查)
```

### 2.2 for 循环

```c
// C代码
for (int i = 0; i < 10; i++) {
    sum += i;
}
```

```asm
# RISC-V汇编
    li   t0, 0              # i = 0
    li   t1, 0              # sum = 0

for_cond:
    li   t2, 10
    bge  t0, t2, for_end    # if (i >= 10) goto end

for_body:
    add  t1, t1, t0         # sum += i

for_inc:
    addi t0, t0, 1          # i++
    j    for_cond           # goto condition

for_end:
    sw   t1, sum
```

```
for循环等价转换:
    for (init; cond; inc) body;

等价于:
    init;
    while (cond) {
        body;
        inc;
    }
```

### 2.3 do-while 循环

```c
// C代码
int i = 0;
do {
    sum += i;
    i++;
} while (i < 10);
```

```asm
# RISC-V汇编（后检查条件，至少执行一次）
    li   t0, 0              # i = 0
    li   t1, 0              # sum = 0

do_body:
    add  t1, t1, t0         # sum += i
    addi t0, t0, 1          # i++

    li   t2, 10
    blt  t0, t2, do_body    # if (i < 10) goto do_body

    sw   t1, sum
```

### 2.4 循环优化：循环展开

```c
// C代码
for (int i = 0; i < 100; i++) {
    sum += arr[i];
}
```

```asm
# RISC-V汇编（展开4次，减少分支预测失败）
    li   t0, 0              # i = 0
    li   t1, 0              # sum = 0
    la   t2, arr            # t2 = &arr[0]

loop:
    lw   t3, 0(t2)          # arr[i]
    add  t1, t1, t3
    lw   t3, 4(t2)          # arr[i+1]
    add  t1, t1, t3
    lw   t3, 8(t2)          # arr[i+2]
    add  t1, t1, t3
    lw   t3, 12(t2)         # arr[i+3]
    add  t1, t1, t3

    addi t2, t2, 16         # 前进4个int
    addi t0, t0, 4          # i += 4
    li   t3, 100
    blt  t0, t3, loop       # if (i < 100) goto loop

    sw   t1, sum
```

---

## 3. switch 语句的汇编实现

### 3.1 跳转表实现（密集case）

```c
// C代码
int opcode = fetch();
int result;
switch (opcode) {
    case 0: result = op_add(); break;
    case 1: result = op_sub(); break;
    case 2: result = op_mul(); break;
    case 3: result = op_div(); break;
    default: result = -1;
}
```

```asm
# RISC-V汇编（跳转表实现）
    la   t0, opcode
    lw   t0, 0(t0)          # t0 = opcode

    # 范围检查
    li   t1, 3
    bgtu t0, t1, default    # if (opcode > 3) goto default

    # 跳转表寻址
    la   t1, jump_table     # t1 = &jump_table[0]
    slli t0, t0, 3          # t0 = opcode * 8 (64位地址)
    add  t1, t1, t0         # t1 = &jump_table[opcode]
    ld   t2, 0(t1)          # t2 = jump_table[opcode]
    jr   t2                 # 跳转到目标

    .data
jump_table:
    .quad case_0
    .quad case_1
    .quad case_2
    .quad case_3

    .text
case_0:
    call op_add
    j    switch_end
case_1:
    call op_sub
    j    switch_end
case_2:
    call op_mul
    j    switch_end
case_3:
    call op_div
    j    switch_end
default:
    li   a0, -1
switch_end:
    sw   a0, result
```

### 3.2 二分查找实现（稀疏case）

```c
// C代码
switch (x) {
    case 1: ...
    case 10: ...
    case 100: ...
    case 1000: ...
}
```

```asm
# RISC-V汇编（二分查找，适合稀疏case）
    lw   t0, x

    # 与中间值比较
    li   t1, 100
    blt  t0, t1, check_low  # if (x < 100) check low half
    bgt  t0, t1, check_high # if (x > 100) check high half
    j    case_100           # x == 100

check_low:
    li   t1, 10
    blt  t0, t1, case_1_or_not
    beq  t0, t1, case_10
    j    default            # 1 < x < 10

case_1_or_not:
    li   t1, 1
    beq  t0, t1, case_1
    j    default

check_high:
    li   t1, 1000
    beq  t0, t1, case_1000
    j    default
```

---

## 4. 函数调用的汇编实现

### 4.1 调用约定概述

```
RISC-V调用约定 (RV64G):
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

寄存器分类:
┌────────────┬────────────────────────────────────┐
│ 寄存器     │ 用途                               │
├────────────┼────────────────────────────────────┤
│ x0 (zero)  │ 恒为0                              │
│ x1 (ra)    │ 返回地址                           │
│ x2 (sp)    │ 栈指针                             │
│ x3 (gp)    │ 全局指针                           │
│ x4 (tp)    │ 线程指针                           │
│ x5-x7      │ 临时寄存器 (t0-t2)                 │
│ x8 (s0/fp) │ 保存寄存器/帧指针                  │
│ x9         │ 保存寄存器 (s1)                    │
│ x10-x17    │ 参数/返回值 (a0-a7)                │
│ x18-x27    │ 保存寄存器 (s2-s11)                │
│ x28-x31    │ 临时寄存器 (t3-t6)                 │
└────────────┴────────────────────────────────────┘

调用者保存: t0-t6, a0-a7
被调者保存: s0-s11
```

### 4.2 简单函数调用

```c
// C代码
int add(int a, int b) {
    return a + b;
}

int main() {
    int x = add(3, 5);
    return x;
}
```

```asm
# RISC-V汇编
    .text
    .globl main

add:                        # 函数入口
    # a0 = 第一个参数 (a)
    # a1 = 第二个参数 (b)
    add  a0, a0, a1         # 返回值 = a + b
    ret                     # jalr x0, x1, 0 (返回)

main:
    addi sp, sp, -16        # 分配栈帧
    sd   ra, 8(sp)          # 保存返回地址（因为main会调用其他函数）

    li   a0, 3              # 第一个参数
    li   a1, 5              # 第二个参数
    call add                # jal ra, add (调用add)

    # 返回值在a0中
    mv   t0, a0             # t0 = 返回值

    ld   ra, 8(sp)          # 恢复返回地址
    addi sp, sp, 16         # 释放栈帧

    mv   a0, t0             # 设置main的返回值
    ret
```

### 4.3 栈帧详解

```
函数调用时的栈帧布局:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

调用者栈帧:
┌────────────────────────┐ ← 调用者sp
│ 局部变量               │
├────────────────────────┤
│ 参数区（如果需要）     │
└────────────────────────┘ ← 调用时sp指向这里
            │
            │ call指令
            ▼
被调者栈帧:
┌────────────────────────┐ ← 新sp（16字节对齐）
│ 保存的ra               │ sp+8
├────────────────────────┤
│ 保存的s0（帧指针）     │ sp+0 (可选)
├────────────────────────┤
│ 局部变量               │
├────────────────────────┤
│ 保存的s1-s11（如果需要）│
└────────────────────────┘
```

```c
// C代码
int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}
```

```asm
# RISC-V汇编（递归函数）
factorial:
    # 序言
    addi sp, sp, -32        # 分配栈帧
    sd   ra, 24(sp)         # 保存返回地址（递归需要！）
    sd   s0, 16(sp)         # 保存s0
    addi s0, sp, 32         # 设置帧指针

    # 保存参数（因为递归调用会修改a0）
    sw   a0, -20(s0)        # 保存n到栈上

    # 基准情况: if (n <= 1)
    li   t0, 1
    ble  a0, t0, base_case

    # 递归情况
    addi a0, a0, -1         # n - 1
    call factorial          # factorial(n-1)

    # 返回值在a0中
    mv   t1, a0             # t1 = factorial(n-1)
    lw   t0, -20(s0)        # t0 = n
    mul  a0, t0, t1         # a0 = n * factorial(n-1)
    j    fact_end

base_case:
    li   a0, 1              # 返回1

fact_end:
    # 尾声
    ld   s0, 16(sp)
    ld   ra, 24(sp)
    addi sp, sp, 32
    ret
```

### 4.4 叶子函数优化

```c
// C代码 - 叶子函数（不调用其他函数）
int square(int x) {
    return x * x;
}
```

```asm
# RISC-V汇编（叶子函数，不需要保存ra）
square:
    # 不需要保存ra，因为不调用其他函数
    # 不需要修改sp
    mul  a0, a0, a0         # 返回值 = x * x
    ret                     # 直接使用ra返回

# 对比非叶子函数:
non_leaf:
    addi sp, sp, -8
    sd   ra, 0(sp)          # 必须保存ra
    call some_function      # 修改了ra
    ld   ra, 0(sp)          # 恢复ra
    addi sp, sp, 8
    ret
```

---

## 5. 尾递归优化

### 5.1 尾递归的概念

```c
// 普通递归（需要保存栈帧）
int factorial(int n, int acc) {
    if (n <= 1) return acc;
    return factorial(n - 1, n * acc);  // 尾调用！
}

// 非尾递归（需要保存状态）
int factorial_bad(int n) {
    if (n <= 1) return 1;
    return n * factorial_bad(n - 1);   // 不是尾调用
}
```

```
尾调用的关键特征：
- 递归调用是函数的最后一个操作
- 调用后不需要保留任何局部状态
- 可以直接复用当前栈帧
```

### 5.2 尾递归优化的汇编实现

```c
// C代码（尾递归阶乘）
int factorial_tail(int n, int acc) {
    if (n <= 1) return acc;
    return factorial_tail(n - 1, n * acc);
}
```

```asm
# RISC-V汇编（启用尾调用优化）
factorial_tail:
    # 不需要保存ra或分配栈帧！
    # 因为递归调用是最后的操作

loop:
    # 参数: a0 = n, a1 = acc
    li   t0, 1
    ble  a0, t0, done       # if (n <= 1) return acc

    mul  a1, a0, a1         # acc = n * acc
    addi a0, a0, -1         # n = n - 1

    j    loop               # 直接跳转，不是call！
    # 等价于: tail call factorial_tail(n-1, n*acc)

done:
    mv   a0, a1             # 返回acc
    ret

# 对比未优化的版本（每次递归都要保存栈帧）:
factorial_no_opt:
    addi sp, sp, -16
    sd   ra, 8(sp)
    sd   a0, 0(sp)          # 保存n

    li   t0, 1
    ble  a0, t0, no_opt_done

    addi a0, a0, -1
    call factorial_no_opt   # 递归调用

    ld   t0, 0(sp)          # 恢复n
    mul  a0, t0, a0         # n * result

no_opt_done:
    ld   ra, 8(sp)
    addi sp, sp, 16
    ret
```

---

## 6. 异常处理与setjmp/longjmp

### 6.1 setjmp/longjmp的汇编实现

```c
// C代码
jmp_buf env;

void func() {
    if (error) {
        longjmp(env, 1);    # 非局部跳转
    }
}

int main() {
    if (setjmp(env) == 0) {
        func();
    } else {
        printf("Error handled\n");
    }
}
```

```asm
# RISC-V汇编（简化版setjmp/longjmp）
# jmp_buf 保存: s0-s11, sp, ra

setjmp:
    # a0 = &jmp_buf
    sd   s0, 0(a0)
    sd   s1, 8(a0)
    # ... 保存s2-s11
    sd   sp, 96(a0)
    sd   ra, 104(a0)

    li   a0, 0              # 返回0
    ret

longjmp:
    # a0 = &jmp_buf, a1 = 返回值
    ld   s0, 0(a0)
    ld   s1, 8(a0)
    # ... 恢复s2-s11
    ld   sp, 96(a0)
    ld   ra, 104(a0)

    mv   a0, a1             # 设置返回值
    bnez a0, ret_now
    li   a0, 1              # 如果返回值为0，设为1
ret_now:
    ret                     # 直接返回到setjmp的位置！
```

---

## 7. 学习检查点

### 7.1 自测问题

**基础理解：**

1. 为什么if-else在汇编中需要用跳转指令实现？
2. for循环和while循环在汇编层面有什么区别？
3. 什么是叶子函数？为什么可以优化栈帧操作？

**进阶分析：**
4. switch语句在什么情况下使用跳转表？什么情况下使用if-else链？
5. 解释尾递归优化的原理。为什么它可以避免栈溢出？
6. 函数调用时，调用者和被调者分别负责保存哪些寄存器？

**动手实践：**
7. 将以下C代码转换为RISC-V汇编：

   ```c
   int gcd(int a, int b) {
       while (b != 0) {
           int t = b;
           b = a % b;
           a = t;
       }
       return a;
   }
   ```

1. 设计一个尾递归优化的fibonacci函数。

### 7.2 答案与解析

<details>
<summary>点击查看答案</summary>

**问题1：**
CPU按顺序执行指令，没有内置的条件执行结构。条件跳转通过改变程序计数器（PC）来实现控制流分支。

**问题2：**
在汇编层面，for和while都转换为条件跳转，结构上几乎相同。区别仅在源代码的语义和代码组织方式。

**问题3：**
叶子函数是不调用其他函数的函数。因为它不修改ra，所以不需要保存/恢复ra，也不需要分配栈帧。

**问题4：**
当case值密集（形成连续或近似连续的序列）时使用跳转表（O(1)时间）。当case值稀疏时使用二分查找或if-else链（避免浪费内存）。

**问题5：**
尾递归优化的关键是递归调用是最后操作，不需要保留当前栈帧状态。因此可以复用当前栈帧，避免栈增长。

**问题6：**

- 调用者保存：t0-t6（临时寄存器）、a0-a7（参数寄存器）
- 被调者保存：s0-s11（保存寄存器）、ra（如果函数会调用其他函数）

**问题7实现：**

```asm
gcd:
    # a0=a, a1=b
gcd_loop:
    beqz a1, gcd_done       # if (b == 0) return a
    mv   t0, a1             # t = b
    rem  a1, a0, a1         # b = a % b
    mv   a0, t0             # a = t
    j    gcd_loop
gcd_done:
    ret                     # 返回值在a0
```

</details>

---

## 8. 与知识库的深层连接

### 8.1 纵向连接

```
理论基础:
└── [λ演算](../21_Computation_Theory/02_Lambda_Calculus.md)
    └── 函数抽象 → 函数调用约定

汇编层:
├── [RISC-V汇编器](./02_RISCV_Assembler.md)
│   └── 控制流指令的编码
│
└── [C到汇编：数据表示](./03_C_to_Assembly_Data.md)
    └── 栈帧中的数据布局

当前层（控制流映射）:
└── 你在这里 → C控制结构 → 汇编跳转

上层抽象:
├── [函数与作用域](../01_Core_Knowledge_System/02_Core_Layer/04_Functions_Scope.md)
│   └── C语言函数语义
│
├── [控制流](../01_Core_Knowledge_System/01_Basic_Layer/04_Control_Flow.md)
│   └── C语言控制结构
│
└── [递归与栈](../01_Core_Knowledge_System/02_Core_Layer/10_Advanced_Data_Structures.md)
    └── 递归的语义和实现
```

### 8.2 横向连接

| 相关文档 | 连接点 |
|:---------|:-------|
| [栈式虚拟机](../23_VM_Implementation/01_Stack_Based_VM.md) | 函数调用的栈模型对比 |
| [调用约定](../01_Core_Knowledge_System/25_C_Interfacing/01_Calling_Conventions.md) | 不同架构的调用约定比较 |
| [编译器优化](../01_Core_Knowledge_System/05_Engineering_Layer/Compiler_Optimization_Deep/01_Loop_Optimization.md) | 循环优化的汇编级实现 |

---

## 9. 延伸阅读

### 9.1 推荐资源

- **RISC-V ABI Specification**: 完整的调用约定规范
- **System V AMD64 ABI**: x86-64的调用约定（与RISC-V对比）
- **《编译器设计》(Dragon Book)**: 第7章 - 运行时环境

### 9.2 实践建议

1. 使用`gdb`单步跟踪函数调用，观察sp和ra的变化
2. 编写纯汇编的递归程序，理解栈的增长
3. 对比启用/禁用优化时编译器生成的汇编代码差异
4. 实现一个简化版的setjmp/longjmp

---

*文档创建: 2026-03-29*
*上次更新: 2026-03-29*
*状态: 核心框架完成*
