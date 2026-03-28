# C到汇编：数据表示

> **层级定位**: 22 Assembler Implementation / 数据映射层
> **前置知识**: [RISC-V汇编器](./02_RISCV_Assembler.md)
> **后续延伸**: [C到汇编：控制流](./04_C_to_Assembly_Control.md)
> **横向关联**: [指针深度](../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md), [数据类型系统](../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md)
> **预计学习时间**: 2周
> **实践验证**: [c_to_asm_data.c](../../examples/assembler/c_to_asm_data.c)

---

## 核心洞察

```
C语言的数据抽象在汇编层面完全消失，只剩下：
- 位模式（存储在寄存器或内存中）
- 地址偏移（指针的实质）
- 连续的存储块（数组和结构体）

理解C到汇编的映射，就是理解：
1. "类型"是编译时的概念，运行时只有字节序列
2. "指针"只是存储地址的整数
3. "结构体"是连续内存的偏移量集合
4. "数组"是基地址+索引×元素大小的计算

这是从高级抽象回到物理现实的旅程。
```

---

## 1. C数据类型的汇编表示

### 1.1 基本整数类型

```
C类型 → 汇编表示映射:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

类型          大小(64位)  寄存器  汇编操作后缀  存储指令
────────────────────────────────────────────────────────
char          1字节       x0-x31  b (byte)     sb/lb
short         2字节       x0-x31  h (half)     sh/lh
int           4字节       x0-x31  w (word)     sw/lw
long          8字节       x0-x31  d (double)   sd/ld
long long     8字节       x0-x31  d            sd/ld

RISC-V RV64I指令:
- lb/lh/lw: 加载后符号扩展
- lbu/lhu/lwu: 加载后零扩展
- sb/sh/sw/sd: 存储（无符号问题）
```

**代码示例：**

```c
// C代码
char c = -1;
short s = -2;
int i = -3;
long l = -4;
```

```asm
# RISC-V汇编 (RV64)
    li   t0, -1
    sb   t0, offset_c(gp)    # char: 只存储低8位

    li   t0, -2
    sh   t0, offset_s(gp)    # short: 存储低16位

    li   t0, -3
    sw   t0, offset_i(gp)    # int: 存储低32位

    li   t0, -4
    sd   t0, offset_l(gp)    # long: 存储64位
```

### 1.2 有符号vs无符号：类型解释

```
关键洞察：
有符号和无共享相同的位模式，区别在于解释方式。

示例：8位值 0xFF
┌────────────────────────────────────────┐
│ 类型      │ 解释          │ 值        │
├───────────┼───────────────┼───────────┤
│ uint8_t   │ 无符号        │ 255       │
│ int8_t    │ 有符号(补码)  │ -1        │
└────────────────────────────────────────┘

汇编层面没有类型检查，只有字节操作！
```

```c
// C代码演示类型转换
uint8_t u = 255;
int8_t s = u;        // s = -1
int i = s;           // i = -1 (符号扩展!)
int j = (int)u;      // j = 255 (零扩展)
```

```asm
# 对应的RISC-V汇编
    lbu  t0, u          # t0 = 255 (零扩展加载)
    sb   t0, s          # 存储回内存（位模式相同）
    lb   t1, s          # t1 = -1 (符号扩展加载!)

    lbu  t2, u          # t2 = 255
    # 将8位扩展到32位
    slli t2, t2, 24     # 左移24位
    srli t2, t2, 24     # 逻辑右移24位 → 零扩展

    lb   t3, s          # t3 = -1 (已符号扩展)
```

### 1.3 浮点类型

```
浮点寄存器：f0-f31 (独立于整数寄存器 x0-x31)

C类型        大小    寄存器    指令集
─────────────────────────────────────────
float        4字节   f0-f31    flw/fsw + 单精度运算
double       8字节   f0-f31    fld/fsd + 双精度运算
long double  16字节  (内存)    软件实现或x86扩展

注意：RV64需要明确的符号扩展指令处理int<->float转换
```

```c
// C代码
double d = 3.14159;
float f = (float)d;
int i = (int)f;
```

```asm
# RISC-V汇编
    la   t0, d_const
    fld  f0, 0(t0)          # 加载double常量
    fsd  f0, d(gp)          # 存储到变量d

    fcvt.s.d f1, f0         # double -> float
    fsw  f1, f(gp)          # 存储float

    fcvt.w.s t0, f1         # float -> int (向零取整)
    sw   t0, i(gp)

.data
d_const: .double 3.14159
```

---

## 2. 指针的汇编本质

### 2.1 指针就是地址

```
C指针概念          汇编实质
─────────────────────────────────
int *p            存储地址的64位(或32位)整数
&x                变量x的地址(编译时常量或栈偏移)
*p                加载p存储的地址处的值
p + 1             p + sizeof(*p) (指针算术)

关键洞察：
"指针类型"仅影响指针算术和加载/存储的大小，
指针变量本身只是一个存储地址的整数。
```

```c
// C代码
int x = 42;
int *p = &x;
int y = *p;
*p = 100;
```

```asm
# RISC-V汇编 (简化，假设x在栈上)
    addi sp, sp, -16        # 分配栈空间

    li   t0, 42
    sw   t0, 12(sp)         # x = 42

    addi t1, sp, 12         # t1 = &x
    sd   t1, 4(sp)          # p = &x (存储地址)

    ld   t2, 4(sp)          # t2 = p (加载地址)
    lw   t3, 0(t2)          # t3 = *p (解引用)
    sw   t3, 0(sp)          # y = *p

    li   t4, 100
    ld   t2, 4(sp)          # t2 = p
    sw   t4, 0(t2)          # *p = 100

    addi sp, sp, 16
```

### 2.2 多级指针

```c
// C代码
int x = 10;
int *p = &x;
int **pp = &p;
int ***ppp = &pp;

int a = ***ppp;   // a = 10
```

```asm
# RISC-V汇编 (简化内存布局)
# 假设栈布局:
# sp+24: x
# sp+16: p (指向x)
# sp+8:  pp (指向p)
# sp+0:  ppp (指向pp)

    addi sp, sp, -32

    # x = 10
    li   t0, 10
    sw   t0, 24(sp)

    # p = &x
    addi t0, sp, 24
    sd   t0, 16(sp)

    # pp = &p
    addi t0, sp, 16
    sd   t0, 8(sp)

    # ppp = &pp
    addi t0, sp, 8
    sd   t0, 0(sp)

    # a = ***ppp
    ld   t0, 0(sp)          # t0 = ppp
    ld   t1, 0(t0)          # t1 = *ppp = pp
    ld   t2, 0(t1)          # t2 = *pp = p
    lw   t3, 0(t2)          # t3 = *p = x = 10
    # t3现在包含10

    addi sp, sp, 32
```

**内存视图：**

```
地址       内容              符号
────────────────────────────────────
sp+24      10                x
sp+16      sp+24 (0x...)     p
sp+8       sp+16 (0x...)     pp
sp+0       sp+8  (0x...)     ppp
```

### 2.3 void* 和类型擦除

```c
// C代码：通用指针操作
void *vp;
int x = 42;
vp = &x;
int *ip = (int*)vp;
```

```asm
# RISC-V汇编
# void* 与 int* 在汇编层面完全相同！
# 区别仅在于C编译器的类型检查

    addi sp, sp, -16

    li   t0, 42
    sw   t0, 12(sp)         # x = 42

    addi t0, sp, 12         # t0 = &x
    sd   t0, 4(sp)          # vp = &x (void*)

    ld   t0, 4(sp)          # t0 = vp
    sd   t0, 0(sp)          # ip = vp (int*)
    # 注意：在汇编中，没有类型转换，只有数据移动

    addi sp, sp, 16
```

---

## 3. 数组的汇编表示

### 3.1 一维数组

```
数组在内存中是连续存储的元素序列。

int arr[5] 的内存布局:
┌─────────┬─────────┬─────────┬─────────┬─────────┐
│ arr[0]  │ arr[1]  │ arr[2]  │ arr[3]  │ arr[4]  │
│ 4字节   │ 4字节   │ 4字节   │ 4字节   │ 4字节   │
└─────────┴─────────┴─────────┴─────────┴─────────┘
↑
&arr[0] = arr (数组名退化为首元素指针)
```

```c
// C代码
int arr[5] = {10, 20, 30, 40, 50};
int x = arr[2];
arr[3] = 100;
int *p = arr;        // p指向arr[0]
int y = *(p + 2);    // y = arr[2] = 30
```

```asm
# RISC-V汇编
    .data
arr:    .word 10, 20, 30, 40, 50    # 初始化数组

    .text
    la   t0, arr            # t0 = &arr[0]
    lw   t1, 8(t0)          # t1 = arr[2] (偏移 = 2*4 = 8)
    # x现在在t1中

    li   t2, 100
    sw   t2, 12(t0)         # arr[3] = 100 (偏移 = 3*4 = 12)

    mv   t3, t0             # p = arr (p存储的是地址)

    # *(p + 2)
    li   t4, 2
    slli t4, t4, 2          # t4 = 2 * 4 = 8 (int的大小)
    add  t5, t3, t4         # t5 = p + 8
    lw   t6, 0(t5)          # t6 = *(p + 2) = 30
    # y现在在t6中
```

### 3.2 数组索引的汇编计算

```
arr[i] 的地址计算：

地址 = base + i * sizeof(element)

RISC-V实现（假设arr是int数组）：
  slli t1, i, 2       # t1 = i * 4 (左移2位 = ×4)
  add  t2, base, t1   # t2 = &arr[i]
  lw   t3, 0(t2)      # t3 = arr[i]

优化：如果数组元素大小是2的幂，使用移位代替乘法
```

```c
// C代码：多维数组索引
double matrix[10][10];
double x = matrix[i][j];
```

```asm
# RISC-V汇编 (64位系统)
# matrix[i][j] = base + (i * 10 + j) * 8

    la   t0, matrix         # t0 = base

    # 计算偏移
    li   t1, 10
    mul  t2, i, t1          # t2 = i * 10
    add  t2, t2, j          # t2 = i * 10 + j
    slli t2, t2, 3          # t2 = (i * 10 + j) * 8 (double = 8字节)

    add  t3, t0, t2         # t3 = &matrix[i][j]
    fld  ft0, 0(t3)         # ft0 = matrix[i][j]
```

### 3.3 多维数组

```
int arr[3][4] 的内存布局（行优先）:
┌────────┬────────┬────────┬────────┬────────┬────────┬───
│[0][0]  │[0][1]  │[0][2]  │[0][3]  │[1][0]  │[1][1]  │...
│4字节   │4字节   │4字节   │4字节   │4字节   │4字节   │
└────────┴────────┴────────┴────────┴────────┴────────┴───

地址计算: arr[i][j] = base + (i * 4 + j) * 4
```

```c
// C代码
int arr[3][4] = {
    {1, 2, 3, 4},
    {5, 6, 7, 8},
    {9, 10, 11, 12}
};
int x = arr[1][2];   // x = 7
```

```asm
# RISC-V汇编
    .data
arr:    .word 1, 2, 3, 4
        .word 5, 6, 7, 8
        .word 9, 10, 11, 12

    .text
    la   t0, arr
    # arr[1][2]: 第1行，第2列
    # 偏移 = (1 * 4 + 2) * 4 = 24
    lw   t1, 24(t0)         # t1 = 7
```

---

## 4. 结构体的汇编表示

### 4.1 基本结构体布局

```c
// C代码
struct Point {
    int x;
    int y;
};

struct Point p = {10, 20};
int a = p.x;
int b = p.y;
```

```asm
# RISC-V汇编
    .data
p:      .word 10            # p.x 在偏移0
        .word 20            # p.y 在偏移4

    .text
    la   t0, p
    lw   t1, 0(t0)          # t1 = p.x = 10
    lw   t2, 4(t0)          # t2 = p.y = 20
```

### 4.2 内存对齐

```
对齐要求：
- 1字节类型 (char): 任意地址
- 2字节类型 (short): 偶数地址
- 4字节类型 (int, float): 4字节对齐
- 8字节类型 (long, double): 8字节对齐

为什么对齐重要？
1. 硬件效率：未对齐访问可能需要多次内存操作
2. 原子性：对齐访问可以是原子的
3. 某些架构（如ARM）不允许未对齐访问
```

```c
// C代码：对齐示例
struct Example {
    char c;      # 1字节，偏移0
    int i;       # 4字节，需要4字节对齐
    char d;      # 1字节
    short s;     # 2字节，需要2字节对齐
};
```

```
内存布局（假设4字节int）：
┌────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┐
│ c  │pad │pad │pad │ i           │ d  │pad │ s         │
│ 0  │ 1  │ 2  │ 3  │ 4  5  6  7  │ 8  │ 9  │ 10 11     │
└────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┘
总大小: 12字节（含3字节填充）

如果重新排序：
struct Optimized {
    int i;       # 偏移0
    short s;     # 偏移4
    char c;      # 偏移6
    char d;      # 偏移7
};
大小: 8字节（无填充！）
```

```asm
# RISC-V汇编：访问带填充的结构体
    la   t0, example
    lb   t1, 0(t0)          # c (偏移0)
    lw   t2, 4(t0)          # i (偏移4，4字节对齐)
    lb   t3, 8(t0)          # d (偏移8)
    lh   t4, 10(t0)         # s (偏移10，2字节对齐)
```

### 4.3 结构体数组

```c
// C代码
struct Point points[3];
points[1].y = 50;
```

```
内存布局 (Point大小 = 8字节):
┌─────────────────┬─────────────────┬─────────────────┐
│ points[0]       │ points[1]       │ points[2]       │
│ x       y       │ x       y       │ x       y       │
│ 0-3     4-7     │ 8-11    12-15   │ 16-19   20-23   │
└─────────────────┴─────────────────┴─────────────────┘

points[1].y 的地址 = base + 1 * 8 + 4 = base + 12
```

```asm
# RISC-V汇编
    la   t0, points
    li   t1, 50
    sw   t1, 12(t0)         # points[1].y = 50
```

---

## 5. 联合体与位域

### 5.1 联合体（Union）

```
联合体所有成员共享同一起始地址，大小等于最大成员。
```

```c
// C代码
union Data {
    int i;
    float f;
    char c[4];
};

union Data d;
d.i = 0x40400000;  // 浮点数 3.0 的IEEE754表示
float x = d.f;      // x = 3.0 (重新解释位模式)
```

```asm
# RISC-V汇编
    .data
d:      .space 4            # 4字节（最大成员int/float的大小）

    .text
    la   t0, d
    li   t1, 0x40400000
    sw   t1, 0(t0)          # d.i = 0x40400000

    flw  ft0, 0(t0)         # 将相同的位模式加载为float
    # ft0现在包含3.0
```

### 5.2 位域（Bit Fields）

```c
// C代码
struct Flags {
    unsigned int flag1 : 1;  # 1位
    unsigned int flag2 : 1;  # 1位
    unsigned int type  : 4;  # 4位
    unsigned int id    : 10; # 10位
};
```

```
内存布局（假设小端，位序从低到高）：
┌────────────────────────────────────────┐
│ id[9:0] │ type[3:0] │ flag2 │ flag1 │  ← 从高位到低位
│ 31   22 │ 21     18 │   17  │   16  │
└────────────────────────────────────────┘

位操作：
flag1 = (word >> 0) & 1
flag2 = (word >> 1) & 1
type  = (word >> 2) & 0xF
id    = (word >> 6) & 0x3FF
```

```asm
# RISC-V汇编：访问位域
    la   t0, flags
    lw   t1, 0(t0)          # 加载整个字

    # 提取 flag1
    andi t2, t1, 1          # t2 = flag1

    # 提取 flag2
    srli t3, t1, 1
    andi t3, t3, 1          # t3 = flag2

    # 提取 type
    srli t4, t1, 2
    andi t4, t4, 0xF        # t4 = type

    # 提取 id
    srli t5, t1, 6
    andi t5, t5, 0x3FF      # t5 = id

    # 设置 type = 5
    li   t6, ~0x3C          # 清除type位的掩码 (0x3C = 0xF << 2)
    and  t1, t1, t6
    li   t6, 5
    slli t6, t6, 2          # t6 = 5 << 2 = 20
    or   t1, t1, t6         # 设置type
    sw   t1, 0(t0)          # 写回
```

---

## 6. 栈与局部变量

### 6.1 栈帧布局

```
函数栈帧（RISC-V调用约定）：
高地址
┌─────────────────────────────┐
│ 返回地址 (ra)               │ ← 如果函数调用其他函数
├─────────────────────────────┤
│ 保存的寄存器 (s0-s11等)     │
├─────────────────────────────┤
│ 局部变量                    │
│   int a;      ← sp+12       │
│   int *p;     ← sp+4        │
│   char c;     ← sp+0        │
├─────────────────────────────┤
│ 参数溢出区（如果需要）      │
├─────────────────────────────┤
│ 调用者栈帧                  │
低地址  ← sp 指向这里
```

```c
// C代码
void func(int a, int b) {
    int x = a + b;      # 局部变量
    int arr[4];          # 局部数组
    arr[0] = x;
}
```

```asm
# RISC-V汇编
func:
    # 序言
    addi sp, sp, -32        # 分配栈帧
    sd   ra, 24(sp)         # 保存返回地址
    sd   s0, 16(sp)         # 保存帧指针
    addi s0, sp, 32         # 设置帧指针

    # 参数在 a0, a1 中
    # x 在 s0-4
    # arr[4] 在 s0-20 到 s0-8

    add  t0, a0, a1         # t0 = a + b
    sw   t0, -4(s0)         # x = t0

    lw   t0, -4(s0)         # t0 = x
    sw   t0, -20(s0)        # arr[0] = x

    # 尾声
    ld   s0, 16(sp)
    ld   ra, 24(sp)
    addi sp, sp, 32
    ret
```

---

## 7. 学习检查点

### 7.1 自测问题

**基础理解：**

1. 为什么`int`和`float`可以存储相同的位模式但有不同的含义？
2. 指针在汇编层面是什么？指针类型信息在运行时还存在吗？
3. 数组名和指针有什么区别？在汇编层面呢？

**进阶分析：**
4. 解释内存对齐的必要性。如果硬件支持未对齐访问，为什么还推荐对齐？
5. 如何计算`arr[i][j]`的地址？与`ptr[i * N + j]`有什么区别？
6. 结构体的位域在汇编层面如何实现？

**动手实践：**
7. 给定结构体：

   ```c
   struct Test {
       char a;
       int b;
       short c;
   };
   ```

   画出其在32位和64位系统上的内存布局（含填充字节）。
8. 编写汇编代码实现：`int sum = arr[0] + arr[1] + arr[2];`

### 7.2 答案与解析

<details>
<summary>点击查看答案</summary>

**问题1：**
类型是编译时概念，用于确定如何解释位模式。`int`使用补码解释，`float`使用IEEE754解释。汇编中只有位模式，没有类型。

**问题2：**
指针是存储内存地址的变量。在汇编中，指针就是存储地址的寄存器或内存位置。指针类型在运行时消失，但在编译时用于确定指针算术的步长和加载/存储的大小。

**问题3：**
数组名是常量（不能赋值），指针是变量。在大多数表达式中，数组名退化为指针。在汇编层面，两者都是地址值。

**问题4：**
对齐提高性能（单次访问vs多次访问），某些操作的原子性要求对齐，某些架构不支持未对齐访问。

**问题5：**
`arr[i][j]` = base + (i *列数 + j)* sizeof(element)
`ptr[i * N + j]` 计算方式相同，但ptr是运行时确定的指针变量。

**问题6：**
位域通过移位和掩码操作实现。编译器生成代码提取和设置特定的位范围。

**问题7：**
32位系统（假设4字节int，2字节short）：

```
| a(1) | pad(3) | b(4) | c(2) | pad(2) | = 12字节
```

64位系统（相同对齐）：

```
| a(1) | pad(3) | b(4) | c(2) | pad(2) | = 12字节
```

优化后顺序：

```
| b(4) | c(2) | a(1) | pad(1) | = 8字节
```

</details>

---

## 8. 与知识库的深层连接

### 8.1 纵向连接

```
理论基础:
└── [图灵机](../21_Computation_Theory/01_Turing_Machines.md)
    └── 存储带抽象 → 内存抽象

物理层:
├── [内存电路](../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/03_Memory_Circuits/README.md)
│   └── 物理存储单元
│
├── [Cache实现](../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/03_Memory_Circuits/04_Cache_Array.md)
│   └── 内存访问的硬件优化
│
└── [MMU](../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/05_Memory_Management_Hardware/README.md)
    └── 虚拟地址到物理地址的转换

当前层（数据表示）:
└── 你在这里 → C数据类型 → 内存布局

上层抽象:
├── [指针深度](../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md)
│   └── C语言的指针语义
│
├── [数组与指针](../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md)
│   └── 数组-指针等价性的真相
│
└── [内存管理](../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md)
    └── 动态内存的分配与释放
```

### 8.2 横向连接

| 相关文档 | 连接点 |
|:---------|:-------|
| [C到汇编：控制流](./04_C_to_Assembly_Control.md) | 函数调用时栈帧的建立 |
| [RISC-V CPU实现](../02_Formal_Semantics_and_Physics/10_ISA_Machine_Code/08_RISCV_CPU_Complete/README.md) | 加载/存储指令的硬件实现 |
| [IEEE 754](../01_Core_Knowledge_System/01_Basic_Layer/IEEE_754_Floating_Point/01_IEEE_754_Basics.md) | 浮点数的位模式解释 |

---

## 9. 延伸阅读

### 9.1 推荐资源

- **System V AMD64 ABI**: 详细的调用约定和内存布局规范
- **RISC-V Calling Convention**: RISC-V的函数调用约定
- **《深入理解计算机系统》(CS:APP)**: 第3章 - 程序的机器级表示

### 9.2 实践建议

1. 使用`objdump -d`查看编译后的汇编代码
2. 使用`gdb`单步跟踪，观察寄存器和内存变化
3. 修改结构体顺序，观察填充字节的变化
4. 编写纯汇编程序操作复杂数据结构

---

*文档创建: 2026-03-29*
*上次更新: 2026-03-29*
*状态: 核心框架完成*
