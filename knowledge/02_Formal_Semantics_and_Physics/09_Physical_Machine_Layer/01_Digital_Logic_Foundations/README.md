# 数字逻辑基础 (Digital Logic Foundations)

> **层级定位**: 02_Formal_Semantics_and_Physics > 09_Physical_Machine_Layer > 01_Digital_Logic_Foundations
> **难度级别**: ⭐⭐⭐⭐ L4
> **前置知识**: 布尔代数、半导体物理基础
> **参考标准**: IEEE Std 1364 (Verilog), IEEE Std 1076 (VHDL)

---

## 目录

- [晶体管级](./01_Transistor_Level.md) - CMOS实现
- [逻辑门实现](./02_Logic_Gates_Implementation.md) - 与或非门
- [组合电路](./03_Combinational_Circuits.md) - 加法器、译码器

---

## 核心概念

数字逻辑是计算机硬件的基础，从晶体管到逻辑门再到复杂电路。

### 布尔代数基础

```
基本运算:
- 与 (AND):  Y = A · B
- 或 (OR):   Y = A + B
- 非 (NOT):  Y = Ā
- 异或 (XOR): Y = A ⊕ B = A·B̄ + Ā·B

德摩根定律:
- A · B = Ā + B̄
- A + B = Ā · B̄
```

### C语言模拟逻辑门

```c
#include <stdint.h>
#include <stdbool.h>

// 基本逻辑门
#define AND(a, b)  ((a) & (b))
#define OR(a, b)   ((a) | (b))
#define NOT(a)     (~(a) & 1)
#define XOR(a, b)  ((a) ^ (b))
#define NAND(a, b) (NOT(AND(a, b)))
#define NOR(a, b)  (NOT(OR(a, b)))

// 多位逻辑运算
uint8_t bitwise_and(uint8_t a, uint8_t b) { return a & b; }
uint8_t bitwise_or(uint8_t a, uint8_t b)  { return a | b; }
uint8_t bitwise_xor(uint8_t a, uint8_t b) { return a ^ b; }
uint8_t bitwise_not(uint8_t a)            { return ~a; }

// 半加器 (Half Adder)
// 输入: a, b
// 输出: sum (和), carry (进位)
typedef struct {
    uint8_t sum;
    uint8_t carry;
} HalfAdderResult;

HalfAdderResult half_adder(uint8_t a, uint8_t b) {
    HalfAdderResult result;
    result.sum = XOR(a, b);
    result.carry = AND(a, b);
    return result;
}

// 全加器 (Full Adder)
// 输入: a, b, cin (进位输入)
// 输出: sum, cout (进位输出)
typedef struct {
    uint8_t sum;
    uint8_t cout;
} FullAdderResult;

FullAdderResult full_adder(uint8_t a, uint8_t b, uint8_t cin) {
    FullAdderResult result;

    uint8_t sum1 = XOR(a, b);
    result.sum = XOR(sum1, cin);

    uint8_t carry1 = AND(a, b);
    uint8_t carry2 = AND(sum1, cin);
    result.cout = OR(carry1, carry2);

    return result;
}

// 4位行波进位加法器 (Ripple Carry Adder)
uint8_t add4(uint8_t a, uint8_t b, uint8_t cin, uint8_t* cout) {
    uint8_t sum = 0;
    uint8_t carry = cin;

    for (int i = 0; i < 4; i++) {
        uint8_t ai = (a >> i) & 1;
        uint8_t bi = (b >> i) & 1;

        FullAdderResult fa = full_adder(ai, bi, carry);
        sum |= (fa.sum << i);
        carry = fa.cout;
    }

    *cout = carry;
    return sum;
}

// 超前进位加法器 (Carry Lookahead Adder) - 更快速
// 生成信号 G = A · B
// 传播信号 P = A ⊕ B
// 进位 C[i+1] = G[i] + P[i] · C[i]
uint8_t cla_add4(uint8_t a, uint8_t b, uint8_t cin, uint8_t* cout) {
    uint8_t g = a & b;    // 生成
    uint8_t p = a ^ b;    // 传播

    // 计算各级进位
    uint8_t c[5];
    c[0] = cin;
    c[1] = g & 1 | (p & 1) & c[0];
    c[2] = (g >> 1) & 1 | ((p >> 1) & 1) & c[1];
    c[3] = (g >> 2) & 1 | ((p >> 2) & 1) & c[2];
    c[4] = (g >> 3) & 1 | ((p >> 3) & 1) & c[3];

    uint8_t sum = p ^ ((c[0]) | (c[1] << 1) | (c[2] << 2) | (c[3] << 3));
    *cout = c[4];

    return sum;
}
```

---

## 深入理解

### 1. CMOS晶体管级实现

```c
// CMOS逻辑门的行为级模拟

// CMOS反相器 (NOT门)
// PMOS + NMOS互补结构
// 输入高 → NMOS导通，输出接地(低)
// 输入低 → PMOS导通，输出接VDD(高)
typedef struct {
    float vdd;      // 电源电压
    float threshold;// 阈值电压
    float output;   // 输出电压
} CMOSInverter;

void cmos_inverter_sim(CMOSInverter* inv, float input) {
    if (input > inv->threshold) {
        // NMOS导通, PMOS截止
        inv->output = 0.0f;  // 接地
    } else {
        // PMOS导通, NMOS截止
        inv->output = inv->vdd;  // 接VDD
    }
}

// 2输入NAND门的晶体管实现
// 上拉网络: 两个PMOS并联
// 下拉网络: 两个NMOS串联
typedef struct {
    float output;
} CMOSNand2;

void cmos_nand2_sim(CMOSNand2* gate, float a, float b, float vdd) {
    // PMOS网络 (上拉)
    // 任一输入为低时,对应PMOS导通
    int pmos_network = (a < 0.5) || (b < 0.5);

    // NMOS网络 (下拉)
    // 两个输入都为高时,NMOS串联导通
    int nmos_network = (a > 0.5) && (b > 0.5);

    if (pmos_network && !nmos_network) {
        gate->output = vdd;  // 上拉
    } else if (!pmos_network && nmos_network) {
        gate->output = 0.0f; // 下拉
    } else {
        // 竞争状态 (瞬态)
        gate->output = vdd / 2;
    }
}

// 延迟模型
// CMOS门延迟与负载电容和晶体管尺寸相关
// t_p ∝ C_load / (W/L)
typedef struct {
    float tplh;  // 低→高传播延迟
    float tphl;  // 高→低传播延迟
} DelayModel;

void calculate_delay(DelayModel* delay, float c_load, float w_nmos, float w_pmos) {
    // 简化RC模型
    float r_nmos = 10.0f / w_nmos;   // 单位宽度电阻
    float r_pmos = 20.0f / w_pmos;   // PMOS电阻约为NMOS的2倍

    delay->tplh = 0.69f * r_pmos * c_load;
    delay->tphl = 0.69f * r_nmos * c_load;
}
```

### 2. 组合电路设计

```c
// 4选1多路选择器 (MUX)
// 选择信号: s[1:0]
// 输入: d0, d1, d2, d3
// 输出: y = s==0 ? d0 : s==1 ? d1 : s==2 ? d2 : d3
uint8_t mux4(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t s) {
    uint8_t sel0 = NOT((s >> 0) & 1);
    uint8_t sel1 = NOT((s >> 1) & 1);
    uint8_t s0 = (s >> 0) & 1;
    uint8_t s1 = (s >> 1) & 1;

    uint8_t term0 = AND(AND(sel1, sel0), d0);
    uint8_t term1 = AND(AND(sel1, s0), d1);
    uint8_t term2 = AND(AND(s1, sel0), d2);
    uint8_t term3 = AND(AND(s1, s0), d3);

    return OR(OR(term0, term1), OR(term2, term3));
}

// 3-8译码器 (Decoder)
// 输入: 3位
// 输出: 8位, 仅对应输入值的位为1
void decoder3to8(uint8_t in, uint8_t* out) {
    uint8_t a = (in >> 0) & 1;
    uint8_t b = (in >> 1) & 1;
    uint8_t c = (in >> 2) & 1;

    *out = 0;
    for (int i = 0; i < 8; i++) {
        uint8_t match = AND(AND(
            XOR(a, (i >> 0) & 1) ^ 1,
            XOR(b, (i >> 1) & 1) ^ 1),
            XOR(c, (i >> 2) & 1) ^ 1);
        *out |= (match << i);
    }
}

// 8-3编码器 (Encoder) - 优先级编码器
uint8_t encoder8to3(uint8_t in) {
    uint8_t out = 0;
    if (in & 0x80) out = 7;
    else if (in & 0x40) out = 6;
    else if (in & 0x20) out = 5;
    else if (in & 0x10) out = 4;
    else if (in & 0x08) out = 3;
    else if (in & 0x04) out = 2;
    else if (in & 0x02) out = 1;
    else if (in & 0x01) out = 0;
    return out;
}

// 比较器 (4位)
typedef struct {
    uint8_t greater;  // A > B
    uint8_t equal;    // A == B
    uint8_t less;     // A < B
} CompareResult;

CompareResult compare4(uint8_t a, uint8_t b) {
    CompareResult result;
    result.equal = (a == b);
    result.greater = (a > b);
    result.less = (a < b);
    return result;
}
```

### 3. Verilog HDL与C的对应

```c
// C代码与其对应的Verilog实现

// C:  if (a > b) y = a; else y = b;
// Verilog: assign y = (a > b) ? a : b;

// 32位ALU核心操作
uint32_t alu32(uint32_t a, uint32_t b, uint8_t alu_op, uint8_t* zero, uint8_t* overflow) {
    uint32_t result = 0;

    switch (alu_op) {
        case 0b0000: // AND
            result = a & b;
            break;
        case 0b0001: // OR
            result = a | b;
            break;
        case 0b0010: // ADD
            result = a + b;
            // 溢出检测: 正+正=负 或 负+负=正
            *overflow = (((int32_t)a > 0 && (int32_t)b > 0 && (int32_t)result < 0) ||
                        ((int32_t)a < 0 && (int32_t)b < 0 && (int32_t)result > 0));
            break;
        case 0b0110: // SUB
            result = a - b;
            break;
        case 0b0111: // SLT (Set Less Than)
            result = ((int32_t)a < (int32_t)b) ? 1 : 0;
            break;
        case 0b1100: // NOR
            result = ~(a | b);
            break;
        default:
            result = 0;
    }

    *zero = (result == 0);
    return result;
}

/*
对应的Verilog实现:

module alu32(
    input [31:0] a, b,
    input [3:0] alu_op,
    output reg [31:0] result,
    output zero, overflow
);
    always @(*) begin
        case (alu_op)
            4'b0000: result = a & b;
            4'b0001: result = a | b;
            4'b0010: {overflow, result} = a + b;
            4'b0110: result = a - b;
            4'b0111: result = (a < b) ? 1 : 0;
            4'b1100: result = ~(a | b);
            default: result = 0;
        endcase
    end

    assign zero = (result == 0);
endmodule
*/
```

---

## 最佳实践

1. **时序分析**: 关键路径分析确保满足建立时间和保持时间
2. **功耗优化**: 时钟门控、电源门控技术降低动态和静态功耗
3. **可测试性**: 扫描链设计便于芯片测试
4. **形式验证**: 使用工具验证电路功能正确性

---

## 权威参考

- [1] "Digital Design and Computer Architecture" by Harris & Harris
- [2] "CMOS VLSI Design" by Weste & Harris
- [3] IEEE Std 1364-2005 (Verilog HDL)
- [4] "Computer Organization and Design" by Patterson & Hennessy

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
> **内容深度**: L4 (专业级)
