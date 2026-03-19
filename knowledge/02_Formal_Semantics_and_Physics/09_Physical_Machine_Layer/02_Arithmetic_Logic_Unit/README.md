# 算术逻辑单元 (ALU - Arithmetic Logic Unit)

> **层级定位**: 02_Formal_Semantics_and_Physics > 09_Physical_Machine_Layer > 02_Arithmetic_Logic_Unit
> **难度级别**: ⭐⭐⭐⭐ L4
> **前置知识**: 数字逻辑基础、二进制运算
> **参考标准**: IEEE 754 (浮点运算), RISC-V ISA

---

## 内容

- [二进制算术](./01_Binary_Arithmetic.md)
- [乘法器和除法器](./02_Multiplier_Divider.md)
- [ALU设计](./03_ALU_Design.md)
- [浮点单元](./04_Floating_Point_Unit.md)

---

## 概述

ALU是CPU的核心组件，负责执行所有算术运算（加、减、乘、除）和逻辑运算（与、或、非、异或）。ALU的设计直接影响处理器的性能和功耗。

### ALU功能分类

| 操作类型 | 指令示例 | 实现复杂度 |
|:---------|:---------|:-----------|
| 逻辑运算 | AND, OR, NOT, XOR | 低 (组合逻辑) |
| 移位运算 | SLL, SRL, SRA | 中 ( barrel shifter) |
| 加减法 | ADD, SUB | 中 (加法器) |
| 比较运算 | SLT, SLTU | 中 (减法 + 符号判断) |
| 乘法 | MUL, MULH | 高 (阵列乘法器) |
| 除法 | DIV, DIVU | 高 (迭代除法器) |

---

## C实现: ALU核心功能

```c
#include <stdint.h>
#include <stdbool.h>

// ALU控制信号定义
#define ALU_OP_AND    0b0000
#define ALU_OP_OR     0b0001
#define ALU_OP_ADD    0b0010
#define ALU_OP_SUB    0b0110
#define ALU_OP_SLT    0b0111  // Set Less Than
#define ALU_OP_NOR    0b1100
#define ALU_OP_XOR    0b1101
#define ALU_OP_SLL    0b0011  // Shift Left Logical
#define ALU_OP_SRL    0b0101  // Shift Right Logical
#define ALU_OP_SRA    0b1001  // Shift Right Arithmetic

// ALU结果结构
typedef struct {
    uint32_t result;      // 运算结果
    bool zero;            // 结果为零标志
    bool overflow;        // 溢出标志
    bool carry;           // 进位标志
    bool negative;        // 负数标志
} ALUResult;

// 32位ALU实现
ALUResult alu32(uint32_t a, uint32_t b, uint8_t alu_op) {
    ALUResult r = {0};
    uint64_t temp = 0;

    switch (alu_op) {
        case ALU_OP_AND:
            r.result = a & b;
            break;

        case ALU_OP_OR:
            r.result = a | b;
            break;

        case ALU_OP_XOR:
            r.result = a ^ b;
            break;

        case ALU_OP_NOR:
            r.result = ~(a | b);
            break;

        case ALU_OP_ADD:
            temp = (uint64_t)a + (uint64_t)b;
            r.result = (uint32_t)temp;
            r.carry = (temp >> 32) & 1;
            // 溢出检测: 同号相加得异号
            r.overflow = (~(a ^ b) & (a ^ r.result)) >> 31;
            break;

        case ALU_OP_SUB:
            temp = (uint64_t)a - (uint64_t)b;
            r.result = (uint32_t)temp;
            r.carry = (temp >> 32) & 1;
            // 溢出检测: 异号相减得与被减数异号
            r.overflow = ((a ^ b) & (a ^ r.result)) >> 31;
            break;

        case ALU_OP_SLT:
            // 有符号比较
            r.result = ((int32_t)a < (int32_t)b) ? 1 : 0;
            break;

        case ALU_OP_SLL:
            r.result = a << (b & 0x1F);  // 只取低5位
            break;

        case ALU_OP_SRL:
            r.result = a >> (b & 0x1F);
            break;

        case ALU_OP_SRA:
            // 算术右移: 保持符号位
            r.result = (uint32_t)((int32_t)a >> (b & 0x1F));
            break;

        default:
            r.result = 0;
    }

    r.zero = (r.result == 0);
    r.negative = (r.result >> 31) & 1;

    return r;
}
```

---

## 深入理解

### 1. 快速加法器设计

```c
// 超前进位加法器 (CLA) - 4位单元
typedef struct {
    uint8_t g;  // 生成信号
    uint8_t p;  // 传播信号
    uint8_t c;  // 进位输出
} CLA4Result;

void cla4_generate(uint8_t a, uint8_t b, uint8_t cin, CLA4Result* r) {
    // G[i] = A[i] & B[i]  (生成进位)
    // P[i] = A[i] ^ B[i]  (传播进位)
    r->g = a & b;
    r->p = a ^ b;

    // 进位计算
    // C[0] = cin
    // C[1] = G[0] | P[0] & C[0]
    // C[2] = G[1] | P[1] & G[0] | P[1] & P[0] & C[0]
    // ...
    r->c = r->g | (r->p & cin);
}

// 16位CLA (使用4个4位CLA单元)
uint16_t cla16_add(uint16_t a, uint16_t b, uint8_t cin, uint8_t* cout) {
    // 分段处理
    uint8_t a0 = a & 0xF, a1 = (a >> 4) & 0xF;
    uint8_t a2 = (a >> 8) & 0xF, a3 = (a >> 12) & 0xF;
    uint8_t b0 = b & 0xF, b1 = (b >> 4) & 0xF;
    uint8_t b2 = (b >> 8) & 0xF, b3 = (b >> 12) & 0xF;

    CLA4Result r0, r1, r2, r3;

    // 计算各段的生成和传播信号
    cla4_generate(a0, b0, cin, &r0);
    cla4_generate(a1, b1, r0.c, &r1);
    cla4_generate(a2, b2, r1.c, &r2);
    cla4_generate(a3, b3, r2.c, &r3);

    // 组间超前进位
    uint8_t g_group = r3.g | (r3.p & r2.g) | (r3.p & r2.p & r1.g) |
                      (r3.p & r2.p & r1.p & r0.g);
    uint8_t p_group = r3.p & r2.p & r1.p & r0.p;

    *cout = g_group | (p_group & cin);

    // 计算和
    uint16_t sum = (a0 ^ b0 ^ cin) |
                   ((a1 ^ b1 ^ r0.c) << 4) |
                   ((a2 ^ b2 ^ r1.c) << 8) |
                   ((a3 ^ b3 ^ r2.c) << 12);

    return sum;
}

// 性能对比
// 行波进位加法器 (RCA): 延迟 O(n)
// 超前进位加法器 (CLA): 延迟 O(log n)
```

### 2. 乘法器实现

```c
// 阵列乘法器 (Array Multiplier) - 无符号32位
uint64_t array_mult32(uint32_t a, uint32_t b) {
    uint64_t result = 0;

    // 逐位部分积累加
    for (int i = 0; i < 32; i++) {
        if ((b >> i) & 1) {
            result += ((uint64_t)a << i);
        }
    }

    return result;
}

// Booth编码乘法器 - 有符号乘法 (更高效)
// Booth算法将乘法次数减少一半
int64_t booth_mult32(int32_t a, int32_t b) {
    int64_t multiplicand = a;
    int64_t product = 0;
    int64_t multiplier = b;

    for (int i = 0; i < 32; i += 2) {
        // 检查当前位和下一位
        int booth_code = ((multiplier >> i) & 1) +
                        (((multiplier >> (i+1)) & 1) << 1) -
                        (((multiplier >> (i-1)) & 1) << 2);

        switch (booth_code) {
            case 0:  // 0: 加0
                break;
            case 1:  // +1: 加被乘数
                product += (multiplicand << i);
                break;
            case 2:  // +1: 同上
                product += (multiplicand << i);
                break;
            case 3:  // +2: 加2倍被乘数
                product += (multiplicand << (i+1));
                break;
            case -1: // -1: 减被乘数
                product -= (multiplicand << i);
                break;
            case -2: // -1: 同上
                product -= (multiplicand << i);
                break;
            case -3: // -2: 减2倍被乘数
                product -= (multiplicand << (i+1));
                break;
        }
    }

    return product;
}

// 流水线乘法器 (高性能处理器)
#define PIPELINE_STAGES 4

typedef struct {
    uint32_t stage_a[PIPELINE_STAGES];
    uint32_t stage_b[PIPELINE_STAGES];
    uint32_t stage_valid[PIPELINE_STAGES];
    uint64_t partial_products[PIPELINE_STAGES];
} PipelineMultiplier;

void pipeline_mult_init(PipelineMultiplier* pm) {
    memset(pm, 0, sizeof(PipelineMultiplier));
}

// 每时钟周期可输入一个新操作数
void pipeline_mult_cycle(PipelineMultiplier* pm, uint32_t a, uint32_t b,
                         uint8_t valid, uint64_t* result, uint8_t* result_valid) {
    // 结果输出
    *result = pm->partial_products[PIPELINE_STAGES-1];
    *result_valid = pm->stage_valid[PIPELINE_STAGES-1];

    // 流水线推进
    for (int i = PIPELINE_STAGES-1; i > 0; i--) {
        pm->stage_a[i] = pm->stage_a[i-1];
        pm->stage_b[i] = pm->stage_b[i-1];
        pm->stage_valid[i] = pm->stage_valid[i-1];
    }

    // 新输入
    pm->stage_a[0] = a;
    pm->stage_b[0] = b;
    pm->stage_valid[0] = valid;

    // 计算部分积 (简化示例)
    pm->partial_products[0] = (uint64_t)pm->stage_a[0] * pm->stage_b[0];
}
```

### 3. 除法器实现

```c
// 恢复余数除法器 (Restoring Division)
typedef struct {
    uint32_t quotient;
    uint32_t remainder;
} DivResult;

DivResult restoring_div32(uint32_t dividend, uint32_t divisor) {
    DivResult r = {0, 0};
    uint64_t rem = dividend;  // 64位余数寄存器
    uint64_t div = ((uint64_t)divisor) << 32;  // 对齐除数

    for (int i = 0; i < 32; i++) {
        // 左移余数
        rem <<= 1;

        // 减去除数
        uint64_t temp = rem - div;

        if (temp >> 63) {
            // 结果为负, 恢复余数
            r.quotient = (r.quotient << 1) | 0;
            // rem保持不变 (恢复)
        } else {
            // 结果为正
            r.quotient = (r.quotient << 1) | 1;
            rem = temp;
        }
    }

    r.remainder = (uint32_t)(rem >> 32);
    return r;
}

// SRT除法器 (现代处理器常用) - 基于查找表
// 每周期产生多位商, 减少迭代次数
// 这里仅展示概念
DivResult srt_div32(uint32_t dividend, uint32_t divisor) {
    // SRT算法使用估计的商数字
    // 需要归一化和查找表
    // 实现较复杂, 这里简化
    return restoring_div32(dividend, divisor);
}
```

---

## 最佳实践

1. **溢出检测**: 始终检查算术运算的溢出条件
2. **符号扩展**: 有符号运算时注意正确的符号扩展
3. **流水线设计**: 高性能处理器使用流水线提高吞吐量
4. **低功耗优化**: 操作数隔离、时钟门控减少动态功耗

---

## 权威参考

- [1] "Computer Arithmetic: Algorithms and Hardware Designs" by Behrooz Parhami
- [2] IEEE Std 754-2019 (IEEE Standard for Floating-Point Arithmetic)
- [3] "Digital Arithmetic" by Milos Ercegovac & Tomas Lang
- [4] RISC-V ISA Specification

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
> **内容深度**: L4 (专业级)
