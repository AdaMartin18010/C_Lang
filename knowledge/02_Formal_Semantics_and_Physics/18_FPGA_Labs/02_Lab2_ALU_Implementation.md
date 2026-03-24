# 实验二：ALU实现实验

## 目录

- [实验二：ALU实现实验](#实验二alu实现实验)
  - [目录](#目录)
  - [1. 实验概述](#1-实验概述)
    - [1.1 ALU在计算机系统中的地位](#11-alu在计算机系统中的地位)
    - [1.2 实验意义](#12-实验意义)
  - [2. 实验目标](#2-实验目标)
    - [2.1 知识目标](#21-知识目标)
    - [2.2 技能目标](#22-技能目标)
    - [2.3 实验成果](#23-实验成果)
  - [3. ALU理论基础](#3-alu理论基础)
    - [3.1 ALU基本结构](#31-alu基本结构)
    - [3.2 算术运算原理](#32-算术运算原理)
      - [3.2.1 加法器设计](#321-加法器设计)
      - [3.2.2 减法器设计（补码加法）](#322-减法器设计补码加法)
    - [3.3 标志位产生逻辑](#33-标志位产生逻辑)
  - [4. 实验环境](#4-实验环境)
    - [4.1 硬件环境](#41-硬件环境)
    - [4.2 资源分配](#42-资源分配)
  - [5. 实验内容](#5-实验内容)
    - [5.1 实验流程](#51-实验流程)
  - [6. Verilog代码详解](#6-verilog代码详解)
    - [6.1 ALU核心模块](#61-alu核心模块)
    - [6.2 改进版ALU（使用显式加法器模块）](#62-改进版alu使用显式加法器模块)
    - [6.3 按钮消抖模块](#63-按钮消抖模块)
    - [6.4 操作码控制模块](#64-操作码控制模块)
    - [6.5 七段数码管多路显示模块](#65-七段数码管多路显示模块)
    - [6.6 顶层模块](#66-顶层模块)
  - [7. 仿真测试](#7-仿真测试)
    - [7.1 Testbench代码](#71-testbench代码)
    - [7.2 仿真运行方法](#72-仿真运行方法)
    - [7.3 预期仿真结果](#73-预期仿真结果)
  - [8. XDC约束文件](#8-xdc约束文件)
  - [9. 烧录与验证](#9-烧录与验证)
    - [9.1 硬件操作流程](#91-硬件操作流程)
    - [9.2 功能测试用例](#92-功能测试用例)
  - [10. 故障排除FAQ](#10-故障排除faq)
    - [Q1: 运算结果不正确](#q1-运算结果不正确)
    - [Q2: 七段数码管显示异常](#q2-七段数码管显示异常)
    - [Q3: 按钮控制不灵敏](#q3-按钮控制不灵敏)
  - [11. 实验报告模板](#11-实验报告模板)
  - [12. 扩展练习](#12-扩展练习)
    - [12.1 8位ALU扩展](#121-8位alu扩展)
    - [12.2 乘除法单元](#122-乘除法单元)
    - [12.3 移位运算](#123-移位运算)
    - [12.4 流水线ALU](#124-流水线alu)
  - [13. 参考资料](#13-参考资料)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## 1. 实验概述

算术逻辑单元（Arithmetic Logic Unit, ALU）是中央处理器（CPU）的核心组件，负责执行所有的算术运算（如加减乘除）和逻辑运算（如与或非异或）。
本实验将指导学习者在FPGA上实现一个功能完整的4位ALU，并通过开发板上的拨码开关、按钮和七段数码管进行交互式验证。

### 1.1 ALU在计算机系统中的地位

```
┌─────────────────────────────────────────────────────────────┐
│                      CPU架构简图                             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   ┌─────────────┐      ┌─────────────────┐                 │
│   │  程序计数器  │      │      ALU        │                 │
│   │     PC      │----->│  ┌───────────┐  │                 │
│   └─────────────┘      │  │  算术单元  │  │                 │
│          │             │  │  + - * /   │  │                 │
│          v             │  ├───────────┤  │                 │
│   ┌─────────────┐      │  │  逻辑单元  │  │                 │
│   │  指令存储器  │      │  │ AND OR XOR │  │                 │
│   │      I$     │----->│  ├───────────┤  │                 │
│   └─────────────┘      │  │  比较单元  │  │                 │
│          │             │  │  < = >     │  │                 │
│          v             │  └───────────┘  │                 │
│   ┌─────────────┐      └────────┬────────┘                 │
│   │  控制单元    │               │                         │
│   │     CU      │<--------------┘                         │
│   └─────────────┘                                          │
│          │                                                 │
│          v                                                 │
│   ┌─────────────┐      ┌─────────────────┐                 │
│   │  寄存器堆    │<---->|     数据存储器   │                 │
│   │     RF      │      │        D$       │                 │
│   └─────────────┘      └─────────────────┘                 │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 实验意义

通过本实验，学习者将：

- 深入理解ALU的内部结构和工作原理
- 掌握多路选择器（MUX）的设计方法
- 理解标志位（Zero/Negative/Carry/Overflow）的产生机制
- 学会设计复杂组合逻辑电路
- 掌握参数化模块设计技巧
- 建立从算法到硬件实现的完整设计流程概念

---

## 2. 实验目标

### 2.1 知识目标

1. **ALU架构理解**：理解ALU的数据通路、运算单元选择机制
2. **算术运算实现**：掌握加法器、减法器的电路实现
3. **标志位机制**：理解并正确实现四种状态标志
4. **参数化设计**：学会使用Verilog参数实现可配置的模块
5. **层次化设计**：掌握模块分解和接口设计原则

### 2.2 技能目标

1. **复杂模块设计**：能够独立完成ALU核心模块的设计
2. **Testbench编写**：编写完整的仿真测试平台进行功能验证
3. **硬件交互设计**：设计人与硬件交互的界面
4. **调试能力**：使用仿真和硬件调试定位问题

### 2.3 实验成果

完成本实验后，学习者将实现一个4位ALU，具备以下功能：

| 操作码 | 操作 | 说明 |
|--------|------|------|
| 000 | ADD | A + B（加法） |
| 001 | SUB | A - B（减法） |
| 010 | AND | A & B（按位与） |
| 011 | OR  | A \| B（按位或） |
| 100 | XOR | A ^ B（按位异或） |
| 101 | NOT | ~A（按位取反） |
| 110 | CMP | A - B（比较，仅更新标志位） |
| 111 | PASS | 直通A到输出 |

**标志位说明：**

- **Zero (Z)**：运算结果为0时置1
- **Negative (N)**：运算结果最高位为1时置1
- **Carry (C)**：加法产生进位或减法产生借位时置1
- **Overflow (V)**：有符号数运算溢出时置1

---

## 3. ALU理论基础

### 3.1 ALU基本结构

```
┌─────────────────────────────────────────────────────────────┐
│                     4位ALU结构框图                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│    A[3:0] ──────┐                                           │
│                 │                                           │
│    B[3:0] ──────┼──> ┌───────────────────┐                 │
│                 │    │                   │                 │
│  ALU_OP[2:0] ───┴──> │      4位ALU        │                 │
│                      │                   │                 │
│                      │  ┌─────────────┐  │                 │
│                      │  │  算术运算单元 │  │                 │
│                      │  │  +  -       │  │                 │
│                      │  └─────────────┘  │                 │
│                      │  ┌─────────────┐  │                 │
│                      │  │  逻辑运算单元 │  │                 │
│                      │  │ & | ^ ~     │  │                 │
│                      │  └─────────────┘  │                 │
│                      │  ┌─────────────┐  │                 │
│                      │  │  多路选择器   │  │                 │
│                      │  │  MUX 8->1   │  │                 │
│                      │  └─────────────┘  │                 │
│                      │                   │                 │
│                      └─────────┬─────────┘                 │
│                                │                           │
│    Result[3:0] <───────────────┘                           │
│                                                             │
│    标志位输出:                                               │
│    Z (Zero) ──────> 运算结果为0                              │
│    N (Negative) ──> 结果为负数                               │
│    C (Carry) ─────> 进位/借位                                │
│    V (Overflow) ──> 有符号溢出                               │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 算术运算原理

#### 3.2.1 加法器设计

```
行波进位加法器（Ripple Carry Adder）:

     A3B3      A2B2      A1B1      A0B0
      │         │         │         │
      v         v         v         v
    ┌───┐     ┌───┐     ┌───┐     ┌───┐
C4<-│FA │<-C3-│FA │<-C2-│FA │<-C1-│FA │<-C0=0
    │   │     │   │     │   │     │   │
    └───┘     └───┘     └───┘     └───┘
      │         │         │         │
     S3        S2        S1        S0

全加器(FA)逻辑:
Sum = A ⊕ B ⊕ Cin
Cout = (A & B) | (A & Cin) | (B & Cin)
```

#### 3.2.2 减法器设计（补码加法）

```
减法 A - B = A + (-B) = A + (~B + 1)

      A3        A2        A1        A0
      │         │         │         │
      v         v         v         v
    ┌───┐     ┌───┐     ┌───┐     ┌───┐
C4<-│FA │<-C3-│FA │<-C2-│FA │<-C1-│FA │<-C0=1 (SUB时)
    │   │     │   │     │   │     │   │
    └───┘     └───┘     └───┘     └───┘
      ↑         ↑         ↑         ↑
    ~B3       ~B2       ~B1       ~B0

注意: SUB操作时，B取反，Cin=1
```

### 3.3 标志位产生逻辑

```
Zero Flag (Z):
Z = ~(Result[3] | Result[2] | Result[1] | Result[0])
结果为0时，Z=1

Negative Flag (N):
N = Result[3]
有符号数运算中，最高位为1表示负数

Carry Flag (C):
C = Cout (来自最高位全加器的进位)
加法: 产生进位时C=1
减法: 产生借位时C=0 (Cout=1表示无借位)

Overflow Flag (V):
V = Cout[3] ⊕ Cout[2]
或者: V = (~A[3] & ~B[3] & Result[3]) | (A[3] & B[3] & ~Result[3]) (加法)
      V = (~A[3] & B[3] & Result[3]) | (A[3] & ~B[3] & ~Result[3]) (减法)
两个正数相加得负数，或两个负数相加得正数时溢出
```

---

## 4. 实验环境

### 4.1 硬件环境

与实验一相同，使用Basys3或Nexys4开发板。

### 4.2 资源分配

```
输入资源分配:
┌─────────────────────────────────────────────────────────────┐
│  拨码开关 (SW0-SW7)  ───> 操作数A[3:0] + 操作数B[3:0]        │
│                                                             │
│  SW0-SW3: 操作数A (4位)                                      │
│  SW4-SW7: 操作数B (4位)                                      │
│                                                             │
│  按钮 (BTNC, BTNU, BTND)  ───> 操作码输入                    │
│                                                             │
│  BTNC: 操作码确认/复位                                       │
│  BTNU: 操作码+1                                              │
│  BTND: 操作码-1                                              │
└─────────────────────────────────────────────────────────────┘

输出资源分配:
┌─────────────────────────────────────────────────────────────┐
│  七段数码管  ───> 显示运算结果 (4位十六进制)                   │
│                                                             │
│  LED (LD0-LD3)  ───> 显示操作数A                             │
│  LED (LD4-LD7)  ───> 显示操作数B                             │
│  LED (LD8-LD11) ───> 显示运算结果                            │
│  LED (LD12-LD15) ───> 显示标志位 Z,N,C,V                     │
└─────────────────────────────────────────────────────────────┘
```

---

## 5. 实验内容

### 5.1 实验流程

```
阶段1: ALU核心模块设计
   ├─ 定义ALU接口和参数
   ├─ 实现算术运算单元（加减法）
   ├─ 实现逻辑运算单元
   ├─ 实现多路选择器
   └─ 实现标志位产生逻辑

阶段2: 输入输出接口设计
   ├─ 设计拨码开关采样模块
   ├─ 设计按钮消抖模块
   ├─ 设计七段数码管驱动模块
   └─ 设计LED显示控制模块

阶段3: 顶层集成
   ├─ 实例化所有子模块
   ├─ 连接数据通路
   └─ 编写顶层约束文件

阶段4: 仿真验证
   ├─ 编写ALU专用testbench
   ├─ 运行功能仿真
   ├─ 验证所有操作和边界条件
   └─ 生成仿真报告

阶段5: 硬件验证
   ├─ 综合实现
   ├─ 生成比特流
   ├─ 下载到FPGA
   └─ 完整功能测试
```

---

## 6. Verilog代码详解

### 6.1 ALU核心模块

```verilog
//////////////////////////////////////////////////////////////////////////////////
// 模块名称: alu_core
// 功能描述: 参数化ALU核心模块，支持多种算术和逻辑运算
// 参数: WIDTH - 数据位宽（默认4位）
// 作者: FPGA Lab
// 版本: 1.0
//////////////////////////////////////////////////////////////////////////////////

`timescale 1ns / 1ps

module alu_core #(
    parameter WIDTH = 4              // 数据位宽，可配置
)(
    input  wire [WIDTH-1:0]   operand_a,   // 操作数A
    input  wire [WIDTH-1:0]   operand_b,   // 操作数B
    input  wire [2:0]         alu_op,      // 操作码
    output reg  [WIDTH-1:0]   result,      // 运算结果
    output reg                flag_z,      // 零标志
    output reg                flag_n,      // 负数标志
    output reg                flag_c,      // 进位/借位标志
    output reg                flag_v       // 溢出标志
);

    ///////////////////////////////////////////////////////////////////////////////
    // 操作码定义
    ///////////////////////////////////////////////////////////////////////////////
    localparam OP_ADD  = 3'b000;     // 加法
    localparam OP_SUB  = 3'b001;     // 减法
    localparam OP_AND  = 3'b010;     // 按位与
    localparam OP_OR   = 3'b011;     // 按位或
    localparam OP_XOR  = 3'b100;     // 按位异或
    localparam OP_NOT  = 3'b101;     // 按位取反
    localparam OP_CMP  = 3'b110;     // 比较（仅更新标志）
    localparam OP_PASS = 3'b111;     // 直通A

    ///////////////////////////////////////////////////////////////////////////////
    // 内部信号
    ///////////////////////////////////////////////////////////////////////////////
    wire [WIDTH-1:0] add_result;
    wire [WIDTH-1:0] sub_result;
    wire [WIDTH-1:0] and_result;
    wire [WIDTH-1:0] or_result;
    wire [WIDTH-1:0] xor_result;
    wire [WIDTH-1:0] not_result;
    wire [WIDTH-1:0] cmp_result;

    wire add_cout;
    wire sub_cout;
    wire sub_vout;

    ///////////////////////////////////////////////////////////////////////////////
    // 算术运算单元 - 加法器
    ///////////////////////////////////////////////////////////////////////////////
    assign {add_cout, add_result} = operand_a + operand_b;

    ///////////////////////////////////////////////////////////////////////////////
    // 算术运算单元 - 减法器（使用补码）
    ///////////////////////////////////////////////////////////////////////////////
    assign {sub_cout, sub_result} = operand_a - operand_b;

    ///////////////////////////////////////////////////////////////////////////////
    // 逻辑运算单元
    ///////////////////////////////////////////////////////////////////////////////
    assign and_result = operand_a & operand_b;
    assign or_result  = operand_a | operand_b;
    assign xor_result = operand_a ^ operand_b;
    assign not_result = ~operand_a;

    // 比较运算（与减法相同，但结果不输出）
    assign cmp_result = sub_result;

    ///////////////////////////////////////////////////////////////////////////////
    // 溢出检测逻辑
    ///////////////////////////////////////////////////////////////////////////////
    // 加法溢出: 正+正=负 或 负+负=正
    wire add_overflow;
    assign add_overflow = (~operand_a[WIDTH-1] & ~operand_b[WIDTH-1] & add_result[WIDTH-1]) |
                          (operand_a[WIDTH-1] & operand_b[WIDTH-1] & ~add_result[WIDTH-1]);

    // 减法溢出: 正-负=负 或 负-正=正
    wire sub_overflow;
    assign sub_overflow = (~operand_a[WIDTH-1] & operand_b[WIDTH-1] & sub_result[WIDTH-1]) |
                          (operand_a[WIDTH-1] & ~operand_b[WIDTH-1] & ~sub_result[WIDTH-1]);

    ///////////////////////////////////////////////////////////////////////////////
    // ALU输出多路选择器
    ///////////////////////////////////////////////////////////////////////////////
    always @(*) begin
        case (alu_op)
            OP_ADD:  result = add_result;
            OP_SUB:  result = sub_result;
            OP_AND:  result = and_result;
            OP_OR:   result = or_result;
            OP_XOR:  result = xor_result;
            OP_NOT:  result = not_result;
            OP_CMP:  result = cmp_result;    // CMP操作也输出结果（调试用）
            OP_PASS: result = operand_a;
            default: result = {WIDTH{1'b0}};
        endcase
    end

    ///////////////////////////////////////////////////////////////////////////////
    // 标志位产生逻辑
    ///////////////////////////////////////////////////////////////////////////////
    always @(*) begin
        // 零标志：结果为全0
        flag_z = (result == {WIDTH{1'b0}});

        // 负数标志：结果最高位为1
        flag_n = result[WIDTH-1];

        // 根据操作类型更新进位和溢出标志
        case (alu_op)
            OP_ADD: begin
                flag_c = add_cout;
                flag_v = add_overflow;
            end

            OP_SUB, OP_CMP: begin
                // 减法时，Cout=1表示无借位，Cout=0表示有借位
                // 但我们通常定义C=1表示有借位，所以取反
                flag_c = ~sub_cout;
                flag_v = sub_overflow;
            end

            default: begin
                // 逻辑运算不影响进位和溢出标志
                flag_c = 1'b0;
                flag_v = 1'b0;
            end
        endcase
    end

endmodule
```

### 6.2 改进版ALU（使用显式加法器模块）

```verilog
//////////////////////////////////////////////////////////////////////////////////
// 模块名称: adder_nbit
// 功能描述: N位行波进位加法器
//////////////////////////////////////////////////////////////////////////////////

module adder_nbit #(
    parameter WIDTH = 4
)(
    input  wire [WIDTH-1:0] a,
    input  wire [WIDTH-1:0] b,
    input  wire             cin,
    output wire [WIDTH-1:0] sum,
    output wire             cout
);

    assign {cout, sum} = a + b + cin;

endmodule

//////////////////////////////////////////////////////////////////////////////////
// 模块名称: alu_core_advanced
// 功能描述: 使用显式加法器模块的ALU实现
//////////////////////////////////////////////////////////////////////////////////

module alu_core_advanced #(
    parameter WIDTH = 4
)(
    input  wire [WIDTH-1:0]   operand_a,
    input  wire [WIDTH-1:0]   operand_b,
    input  wire [2:0]         alu_op,
    output reg  [WIDTH-1:0]   result,
    output reg                flag_z,
    output reg                flag_n,
    output reg                flag_c,
    output reg                flag_v
);

    // 操作码定义
    localparam OP_ADD  = 3'b000;
    localparam OP_SUB  = 3'b001;
    localparam OP_AND  = 3'b010;
    localparam OP_OR   = 3'b011;
    localparam OP_XOR  = 3'b100;
    localparam OP_NOT  = 3'b101;
    localparam OP_CMP  = 3'b110;
    localparam OP_PASS = 3'b111;

    // 内部信号
    wire [WIDTH-1:0] arith_result;
    wire [WIDTH-1:0] logic_result;
    wire [WIDTH-1:0] pass_result;
    wire             arith_cout;
    wire             is_sub;

    // 判断是否为减法类操作
    assign is_sub = (alu_op == OP_SUB) || (alu_op == OP_CMP);

    // 算术运算：加法器复用
    // 减法时，B取反，Cin=1
    adder_nbit #(.WIDTH(WIDTH)) u_adder (
        .a    (operand_a),
        .b    (is_sub ? ~operand_b : operand_b),
        .cin  (is_sub),
        .sum  (arith_result),
        .cout (arith_cout)
    );

    // 逻辑运算
    always @(*) begin
        case (alu_op)
            OP_AND: logic_result = operand_a & operand_b;
            OP_OR:  logic_result = operand_a | operand_b;
            OP_XOR: logic_result = operand_a ^ operand_b;
            OP_NOT: logic_result = ~operand_a;
            default: logic_result = {WIDTH{1'b0}};
        endcase
    end

    // 直通
    assign pass_result = operand_a;

    // 最终结果多路选择
    always @(*) begin
        case (alu_op)
            OP_ADD, OP_SUB, OP_CMP: result = arith_result;
            OP_AND, OP_OR, OP_XOR, OP_NOT: result = logic_result;
            OP_PASS: result = pass_result;
            default: result = {WIDTH{1'b0}};
        endcase
    end

    // 标志位计算
    always @(*) begin
        // 零标志
        flag_z = (result == {WIDTH{1'b0}});

        // 负数标志
        flag_n = result[WIDTH-1];

        // 进位和溢出标志（仅算术运算有效）
        if (alu_op == OP_ADD) begin
            flag_c = arith_cout;
            flag_v = (~operand_a[WIDTH-1] & ~operand_b[WIDTH-1] & result[WIDTH-1]) |
                     (operand_a[WIDTH-1] & operand_b[WIDTH-1] & ~result[WIDTH-1]);
        end
        else if (alu_op == OP_SUB || alu_op == OP_CMP) begin
            flag_c = ~arith_cout;  // 减法时借位与进位标志相反
            flag_v = (~operand_a[WIDTH-1] & operand_b[WIDTH-1] & result[WIDTH-1]) |
                     (operand_a[WIDTH-1] & ~operand_b[WIDTH-1] & ~result[WIDTH-1]);
        end
        else begin
            flag_c = 1'b0;
            flag_v = 1'b0;
        end
    end

endmodule
```

### 6.3 按钮消抖模块

```verilog
//////////////////////////////////////////////////////////////////////////////////
// 模块名称: button_debounce
// 功能描述: 按钮硬件消抖模块
//////////////////////////////////////////////////////////////////////////////////

`timescale 1ns / 1ps

module button_debounce (
    input  wire clk,           // 100MHz时钟
    input  wire rst_n,         // 低电平复位
    input  wire btn_in,        // 按钮输入
    output reg  btn_out,       // 消抖后输出
    output reg  btn_posedge,   // 上升沿脉冲
    output reg  btn_negedge    // 下降沿脉冲
);

    // 参数定义
    localparam DEBOUNCE_CNT = 20'd1_000_000;  // 10ms @ 100MHz

    // 内部信号
    reg btn_in_sync1, btn_in_sync2;
    reg [19:0] cnt;
    reg cnt_en;
    reg btn_stable;

    // 同步器：消除亚稳态
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            btn_in_sync1 <= 1'b0;
            btn_in_sync2 <= 1'b0;
        end
        else begin
            btn_in_sync1 <= btn_in;
            btn_in_sync2 <= btn_in_sync1;
        end
    end

    // 状态机实现消抖
    localparam IDLE  = 2'b00;
    localparam COUNT = 2'b01;
    localparam STABLE = 2'b10;

    reg [1:0] state, next_state;

    // 状态寄存器
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n)
            state <= IDLE;
        else
            state <= next_state;
    end

    // 下一状态逻辑
    always @(*) begin
        next_state = state;
        case (state)
            IDLE: begin
                if (btn_in_sync2)
                    next_state = COUNT;
            end
            COUNT: begin
                if (!btn_in_sync2)
                    next_state = IDLE;
                else if (cnt == DEBOUNCE_CNT - 1)
                    next_state = STABLE;
            end
            STABLE: begin
                if (!btn_in_sync2)
                    next_state = IDLE;
            end
            default: next_state = IDLE;
        endcase
    end

    // 输出逻辑
    always @(*) begin
        cnt_en = (state == COUNT);
        btn_stable = (state == STABLE);
    end

    // 计数器
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n)
            cnt <= 20'd0;
        else if (cnt_en)
            cnt <= cnt + 1'b1;
        else
            cnt <= 20'd0;
    end

    // 稳定输出
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n)
            btn_out <= 1'b0;
        else
            btn_out <= btn_stable;
    end

    // 边沿检测
    reg btn_stable_d;
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n)
            btn_stable_d <= 1'b0;
        else
            btn_stable_d <= btn_stable;
    end

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            btn_posedge <= 1'b0;
            btn_negedge <= 1'b0;
        end
        else begin
            btn_posedge <= btn_stable && !btn_stable_d;
            btn_negedge <= !btn_stable && btn_stable_d;
        end
    end

endmodule
```

### 6.4 操作码控制模块

```verilog
//////////////////////////////////////////////////////////////////////////////////
// 模块名称: op_code_controller
// 功能描述: 操作码选择与显示控制
//////////////////////////////////////////////////////////////////////////////////

`timescale 1ns / 1ps

module op_code_controller (
    input  wire       clk,
    input  wire       rst_n,
    input  wire       btn_up,       // 增加操作码
    input  wire       btn_down,     // 减少操作码
    input  wire       btn_confirm,  // 确认操作
    output reg  [2:0] alu_op,       // 当前操作码
    output reg  [3:0] op_display,   // 操作显示
    output reg        op_valid      // 操作有效指示
);

    // 操作码名称定义（用于显示）
    localparam DISP_ADD  = 4'h0;
    localparam DISP_SUB  = 4'h1;
    localparam DISP_AND  = 4'h2;
    localparam DISP_OR   = 4'h3;
    localparam DISP_XOR  = 4'h4;
    localparam DISP_NOT  = 4'h5;
    localparam DISP_CMP  = 4'h6;
    localparam DISP_PASS = 4'h7;

    // 操作码寄存器
    reg [2:0] op_code_reg;
    reg [2:0] op_code_next;

    // 边沿检测
    reg btn_up_d, btn_down_d;
    wire btn_up_posedge, btn_down_posedge;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            btn_up_d <= 1'b0;
            btn_down_d <= 1'b0;
        end
        else begin
            btn_up_d <= btn_up;
            btn_down_d <= btn_down;
        end
    end

    assign btn_up_posedge = btn_up && !btn_up_d;
    assign btn_down_posedge = btn_down && !btn_down_d;

    // 操作码更新逻辑
    always @(*) begin
        op_code_next = op_code_reg;
        if (btn_up_posedge)
            op_code_next = op_code_reg + 1'b1;
        else if (btn_down_posedge)
            op_code_next = op_code_reg - 1'b1;
    end

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n)
            op_code_reg <= 3'b000;
        else
            op_code_reg <= op_code_next;
    end

    // 输出操作码
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n)
            alu_op <= 3'b000;
        else if (btn_confirm)
            alu_op <= op_code_reg;
    end

    // 操作显示译码
    always @(*) begin
        case (op_code_reg)
            3'b000: op_display = DISP_ADD;
            3'b001: op_display = DISP_SUB;
            3'b010: op_display = DISP_AND;
            3'b011: op_display = DISP_OR;
            3'b100: op_display = DISP_XOR;
            3'b101: op_display = DISP_NOT;
            3'b110: op_display = DISP_CMP;
            3'b111: op_display = DISP_PASS;
            default: op_display = 4'hF;
        endcase
    end

    // 操作有效指示（闪烁）
    reg [24:0] blink_cnt;
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n)
            blink_cnt <= 25'd0;
        else
            blink_cnt <= blink_cnt + 1'b1;
    end

    always @(*) begin
        op_valid = blink_cnt[24];
    end

endmodule
```

### 6.5 七段数码管多路显示模块

```verilog
//////////////////////////////////////////////////////////////////////////////////
// 模块名称: seg7_quad_display
// 功能描述: 4位七段数码管多路显示ALU相关信息
//////////////////////////////////////////////////////////////////////////////////

`timescale 1ns / 1ps

module seg7_quad_display (
    input  wire        clk,
    input  wire        rst_n,
    input  wire [3:0]  display_a,      // 显示操作数A
    input  wire [3:0]  display_b,      // 显示操作数B
    input  wire [3:0]  display_result, // 显示结果
    input  wire [3:0]  display_op,     // 显示操作码
    input  wire [1:0]  display_mode,   // 显示模式选择
    output reg  [6:0]  seg,
    output reg  [3:0]  an
);

    // 扫描参数
    localparam SCAN_DIV = 17'd100_000;  // 1ms @ 100MHz

    // 七段译码表（共阳极，低电平有效）
    function [6:0] hex7seg;
        input [3:0] hex;
        case (hex)
            4'd0:  hex7seg = 7'b100_0000;
            4'd1:  hex7seg = 7'b111_1001;
            4'd2:  hex7seg = 7'b010_0100;
            4'd3:  hex7seg = 7'b011_0000;
            4'd4:  hex7seg = 7'b001_1001;
            4'd5:  hex7seg = 7'b001_0010;
            4'd6:  hex7seg = 7'b000_0010;
            4'd7:  hex7seg = 7'b111_1000;
            4'd8:  hex7seg = 7'b000_0000;
            4'd9:  hex7seg = 7'b001_0000;
            4'd10: hex7seg = 7'b000_1000;  // A
            4'd11: hex7seg = 7'b000_0011;  // b
            4'd12: hex7seg = 7'b100_0110;  // C
            4'd13: hex7seg = 7'b010_0001;  // d
            4'd14: hex7seg = 7'b000_0110;  // E
            4'd15: hex7seg = 7'b000_1110;  // F
            default: hex7seg = 7'b111_1111;
        endcase
    endfunction

    // 扫描计数器和位选择
    reg [16:0] scan_cnt;
    reg [1:0]  digit_sel;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            scan_cnt <= 17'd0;
            digit_sel <= 2'd0;
        end
        else if (scan_cnt == SCAN_DIV - 1) begin
            scan_cnt <= 17'd0;
            digit_sel <= digit_sel + 1'b1;
        end
        else begin
            scan_cnt <= scan_cnt + 1'b1;
        end
    end

    // 位选和段选逻辑
    always @(*) begin
        case (digit_sel)
            2'd0: begin
                an = 4'b1110;
                case (display_mode)
                    2'b00: seg = hex7seg(display_result);  // 模式0: 显示结果
                    2'b01: seg = hex7seg(display_a);       // 模式1: 显示A
                    2'b10: seg = hex7seg(display_b);       // 模式2: 显示B
                    2'b11: seg = hex7seg(display_op);      // 模式3: 显示操作
                endcase
            end
            2'd1: begin
                an = 4'b1101;
                case (display_mode)
                    2'b00: seg = hex7seg(display_a);       // 结果模式下显示A
                    default: seg = 7'b111_1111;
                endcase
            end
            2'd2: begin
                an = 4'b1011;
                case (display_mode)
                    2'b00: seg = hex7seg(display_b);       // 结果模式下显示B
                    default: seg = 7'b111_1111;
                endcase
            end
            2'd3: begin
                an = 4'b0111;
                case (display_mode)
                    2'b00: seg = hex7seg(display_op);      // 结果模式下显示操作
                    default: seg = 7'b111_1111;
                endcase
            end
            default: begin
                an = 4'b1111;
                seg = 7'b111_1111;
            end
        endcase
    end

endmodule
```

### 6.6 顶层模块

```verilog
//////////////////////////////////////////////////////////////////////////////////
// 模块名称: alu_top
// 功能描述: ALU实验顶层模块
//////////////////////////////////////////////////////////////////////////////////

`timescale 1ns / 1ps

module alu_top (
    // 系统信号
    input  wire        clk,       // 100MHz (W5)
    input  wire        btnC,      // 中心按钮-复位
    input  wire        btnU,      // 上按钮-操作码+
    input  wire        btnD,      // 下按钮-操作码-

    // 拨码开关
    input  wire [15:0] sw,

    // LED输出
    output wire [15:0] led,

    // 七段数码管
    output wire [6:0]  seg,
    output wire [3:0]  an
);

    ///////////////////////////////////////////////////////////////////////////////
    // 信号定义
    ///////////////////////////////////////////////////////////////////////////////
    wire rst_n = ~btnC;

    // 操作数
    wire [3:0] operand_a = sw[3:0];
    wire [3:0] operand_b = sw[7:4];

    // ALU接口
    wire [2:0] alu_op;
    wire [3:0] alu_result;
    wire flag_z, flag_n, flag_c, flag_v;

    // 按钮消抖后信号
    wire btn_up_stable;
    wire btn_down_stable;
    wire btn_confirm;

    // 显示控制
    wire [3:0] op_display;
    reg  [1:0] display_mode;
    wire op_valid;

    // 模式切换计数器
    reg [25:0] mode_cnt;
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n)
            mode_cnt <= 26'd0;
        else
            mode_cnt <= mode_cnt + 1'b1;
    end

    // 每2秒切换一次显示模式
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n)
            display_mode <= 2'b00;
        else if (mode_cnt == 26'd99_999_999)
            display_mode <= display_mode + 1'b1;
    end

    ///////////////////////////////////////////////////////////////////////////////
    // 实例化：按钮消抖
    ///////////////////////////////////////////////////////////////////////////////
    button_debounce u_btn_up (
        .clk         (clk),
        .rst_n       (rst_n),
        .btn_in      (btnU),
        .btn_out     (btn_up_stable),
        .btn_posedge (),
        .btn_negedge ()
    );

    button_debounce u_btn_down (
        .clk         (clk),
        .rst_n       (rst_n),
        .btn_in      (btnD),
        .btn_out     (btn_down_stable),
        .btn_posedge (),
        .btn_negedge ()
    );

    // 使用开关SW15作为确认按钮
    wire btn_confirm_raw = sw[15];
    button_debounce u_btn_confirm (
        .clk         (clk),
        .rst_n       (rst_n),
        .btn_in      (btn_confirm_raw),
        .btn_out     (),
        .btn_posedge (btn_confirm),
        .btn_negedge ()
    );

    ///////////////////////////////////////////////////////////////////////////////
    // 实例化：操作码控制器
    ///////////////////////////////////////////////////////////////////////////////
    op_code_controller u_op_ctrl (
        .clk         (clk),
        .rst_n       (rst_n),
        .btn_up      (btn_up_stable),
        .btn_down    (btn_down_stable),
        .btn_confirm (btn_confirm),
        .alu_op      (alu_op),
        .op_display  (op_display),
        .op_valid    (op_valid)
    );

    ///////////////////////////////////////////////////////////////////////////////
    // 实例化：ALU核心
    ///////////////////////////////////////////////////////////////////////////////
    alu_core #(
        .WIDTH(4)
    ) u_alu (
        .operand_a (operand_a),
        .operand_b (operand_b),
        .alu_op    (alu_op),
        .result    (alu_result),
        .flag_z    (flag_z),
        .flag_n    (flag_n),
        .flag_c    (flag_c),
        .flag_v    (flag_v)
    );

    ///////////////////////////////////////////////////////////////////////////////
    // 实例化：七段数码管显示
    ///////////////////////////////////////////////////////////////////////////////
    seg7_quad_display u_display (
        .clk           (clk),
        .rst_n         (rst_n),
        .display_a     (operand_a),
        .display_b     (operand_b),
        .display_result(alu_result),
        .display_op    (op_display),
        .display_mode  (display_mode),
        .seg           (seg),
        .an            (an)
    );

    ///////////////////////////////////////////////////////////////////////////////
    // LED输出分配
    ///////////////////////////////////////////////////////////////////////////////
    assign led[3:0]  = operand_a;      // LD0-3: 操作数A
    assign led[7:4]  = operand_b;      // LD4-7: 操作数B
    assign led[11:8] = alu_result;     // LD8-11: 运算结果
    assign led[12]   = flag_z;         // LD12: 零标志
    assign led[13]   = flag_n;         // LD13: 负标志
    assign led[14]   = flag_c;         // LD14: 进位标志
    assign led[15]   = flag_v;         // LD15: 溢出标志

endmodule
```

---

## 7. 仿真测试

### 7.1 Testbench代码

```verilog
//////////////////////////////////////////////////////////////////////////////////
// 测试文件: tb_alu_core.v
// 功能描述: ALU核心模块的完整测试平台
//////////////////////////////////////////////////////////////////////////////////

`timescale 1ns / 1ps

module tb_alu_core;

    ///////////////////////////////////////////////////////////////////////////////
    // 参数和信号定义
    ///////////////////////////////////////////////////////////////////////////////
    parameter WIDTH = 4;

    reg  [WIDTH-1:0] operand_a;
    reg  [WIDTH-1:0] operand_b;
    reg  [2:0]       alu_op;
    wire [WIDTH-1:0] result;
    wire             flag_z;
    wire             flag_n;
    wire             flag_c;
    wire             flag_v;

    // 操作码定义
    localparam OP_ADD  = 3'b000;
    localparam OP_SUB  = 3'b001;
    localparam OP_AND  = 3'b010;
    localparam OP_OR   = 3'b011;
    localparam OP_XOR  = 3'b100;
    localparam OP_NOT  = 3'b101;
    localparam OP_CMP  = 3'b110;
    localparam OP_PASS = 3'b111;

    ///////////////////////////////////////////////////////////////////////////////
    // 实例化待测模块
    ///////////////////////////////////////////////////////////////////////////////
    alu_core #(
        .WIDTH(WIDTH)
    ) uut (
        .operand_a (operand_a),
        .operand_b (operand_b),
        .alu_op    (alu_op),
        .result    (result),
        .flag_z    (flag_z),
        .flag_n    (flag_n),
        .flag_c    (flag_c),
        .flag_v    (flag_v)
    );

    ///////////////////////////////////////////////////////////////////////////////
    // 任务：显示测试结果
    ///////////////////////////////////////////////////////////////////////////////
    task display_result;
        input [8*20:1] op_name;
        begin
            $display("%s: A=%h, B=%h, Result=%h, Flags={Z=%b,N=%b,C=%b,V=%b}",
                     op_name, operand_a, operand_b, result, flag_z, flag_n, flag_c, flag_v);
        end
    endtask

    ///////////////////////////////////////////////////////////////////////////////
    // 测试序列
    ///////////////////////////////////////////////////////////////////////////////
    initial begin
        $display("========================================");
        $display("      ALU Core Testbench Started       ");
        $display("========================================");

        // 初始化
        operand_a = 4'd0;
        operand_b = 4'd0;
        alu_op = OP_PASS;
        #10;

        //--------------------------------------------------
        // 测试1: 加法运算
        //--------------------------------------------------
        $display("\n--- Test 1: Addition ---");
        alu_op = OP_ADD;

        // 基本加法
        operand_a = 4'd5; operand_b = 4'd3; #10;
        display_result("ADD (5+3)");

        // 加法进位
        operand_a = 4'd10; operand_b = 4'd7; #10;
        display_result("ADD (10+7)");

        // 加法溢出（有符号）
        operand_a = 4'd7; operand_b = 4'd7; #10;  // 7+7=14 (>7, 正溢出)
        display_result("ADD (7+7) - Overflow");

        // 结果为0
        operand_a = 4'd8; operand_b = 4'd8; #10;  // 8+8=0 (模16)
        display_result("ADD (8+8) = 0");

        // 负数加法
        operand_a = 4'b1111; operand_b = 4'b0001; #10;  // -1 + 1 = 0
        display_result("ADD (-1+1) = 0");

        //--------------------------------------------------
        // 测试2: 减法运算
        //--------------------------------------------------
        $display("\n--- Test 2: Subtraction ---");
        alu_op = OP_SUB;

        // 基本减法
        operand_a = 4'd9; operand_b = 4'd4; #10;
        display_result("SUB (9-4)");

        // 减法借位
        operand_a = 4'd3; operand_b = 4'd7; #10;
        display_result("SUB (3-7) - Borrow");

        // 结果为0
        operand_a = 4'd5; operand_b = 4'd5; #10;
        display_result("SUB (5-5) = 0");

        // 负数减法
        operand_a = 4'b1111; operand_b = 4'b0001; #10;  // -1 - 1 = -2
        display_result("SUB (-1-1) = -2");

        //--------------------------------------------------
        // 测试3: 逻辑运算
        //--------------------------------------------------
        $display("\n--- Test 3: Logic Operations ---");
        operand_a = 4'b1010;
        operand_b = 4'b1100;

        alu_op = OP_AND; #10;
        display_result("AND (1010 & 1100)");

        alu_op = OP_OR; #10;
        display_result("OR  (1010 | 1100)");

        alu_op = OP_XOR; #10;
        display_result("XOR (1010 ^ 1100)");

        alu_op = OP_NOT; #10;
        display_result("NOT (~1010)");

        //--------------------------------------------------
        // 测试4: 比较运算
        //--------------------------------------------------
        $display("\n--- Test 4: Compare ---");
        alu_op = OP_CMP;

        operand_a = 4'd7; operand_b = 4'd5; #10;
        display_result("CMP (7 vs 5)");

        operand_a = 4'd3; operand_b = 4'd9; #10;
        display_result("CMP (3 vs 9)");

        operand_a = 4'd6; operand_b = 4'd6; #10;
        display_result("CMP (6 vs 6)");

        //--------------------------------------------------
        // 测试5: 直通运算
        //--------------------------------------------------
        $display("\n--- Test 5: Pass Through ---");
        alu_op = OP_PASS;
        operand_a = 4'b1010; #10;
        display_result("PASS (A)");

        //--------------------------------------------------
        // 边界条件测试
        //--------------------------------------------------
        $display("\n--- Test 6: Boundary Conditions ---");

        // 最大值加法
        alu_op = OP_ADD;
        operand_a = 4'd15; operand_b = 4'd15; #10;
        display_result("ADD (15+15)");

        // 零值运算
        operand_a = 4'd0; operand_b = 4'd0; #10;
        display_result("ADD (0+0)");

        // 自减
        alu_op = OP_SUB;
        operand_a = 4'd8; operand_b = 4'd8; #10;
        display_result("SUB (8-8)");

        // 全1逻辑运算
        alu_op = OP_AND;
        operand_a = 4'b1111; operand_b = 4'b1111; #10;
        display_result("AND (F&F)");

        $display("\n========================================");
        $display("      ALU Core Testbench Finished      ");
        $display("========================================");

        $finish;
    end

    ///////////////////////////////////////////////////////////////////////////////
    // 波形生成
    ///////////////////////////////////////////////////////////////////////////////
    initial begin
        $dumpfile("alu_core.vcd");
        $dumpvars(0, tb_alu_core);
    end

endmodule
```

### 7.2 仿真运行方法

```tcl
# Vivado仿真Tcl脚本
---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/readme.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/readme.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/readme.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/readme.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/readme.md) | 核心关联 | 思维表征基础 |
# 保存为 run_sim.tcl

# 创建工程
set project_name "alu_sim"
create_project $project_name ./$project_name -force

# 添加源文件
add_files -fileset sources_1 {
    ../src/alu_core.v
}

# 添加测试文件
add_files -fileset sim_1 {
    ../sim/tb_alu_core.v
}

# 设置顶层
set_property top tb_alu_core [get_filesets sim_1]

# 运行仿真
launch_simulation
run all
```

### 7.3 预期仿真结果

```
========================================
      ALU Core Testbench Started
========================================

--- Test 1: Addition ---
ADD (5+3): A=5, B=3, Result=8, Flags={Z=0,N=0,C=0,V=0}
ADD (10+7): A=a, B=7, Result=1, Flags={Z=0,N=0,C=1,V=0}
ADD (7+7) - Overflow: A=7, B=7, Result=e, Flags={Z=0,N=1,C=0,V=1}
ADD (8+8) = 0: A=8, B=8, Result=0, Flags={Z=1,N=0,C=1,V=1}
ADD (-1+1) = 0: A=f, B=1, Result=0, Flags={Z=1,N=0,C=1,V=0}

--- Test 2: Subtraction ---
SUB (9-4): A=9, B=4, Result=5, Flags={Z=0,N=0,C=0,V=0}
SUB (3-7) - Borrow: A=3, B=7, Result=c, Flags={Z=0,N=1,C=1,V=0}
SUB (5-5) = 0: A=5, B=5, Result=0, Flags={Z=1,N=0,C=0,V=0}
SUB (-1-1) = -2: A=f, B=1, Result=e, Flags={Z=0,N=1,C=0,V=0}

--- Test 3: Logic Operations ---
AND (1010 & 1100): A=a, B=c, Result=8, Flags={Z=0,N=1,C=0,V=0}
OR  (1010 | 1100): A=a, B=c, Result=e, Flags={Z=0,N=1,C=0,V=0}
XOR (1010 ^ 1100): A=a, B=c, Result=6, Flags={Z=0,N=0,C=0,V=0}
NOT (~1010): A=a, B=c, Result=5, Flags={Z=0,N=0,C=0,V=0}

--- Test 4: Compare ---
CMP (7 vs 5): A=7, B=5, Result=2, Flags={Z=0,N=0,C=0,V=0}
CMP (3 vs 9): A=3, B=9, Result=a, Flags={Z=0,N=1,C=1,V=0}
CMP (6 vs 6): A=6, B=6, Result=0, Flags={Z=1,N=0,C=0,V=0}

--- Test 5: Pass Through ---
PASS (A): A=a, B=c, Result=a, Flags={Z=0,N=1,C=0,V=0}

--- Test 6: Boundary Conditions ---
ADD (15+15): A=f, B=f, Result=e, Flags={Z=0,N=1,C=1,V=0}
ADD (0+0): A=0, B=0, Result=0, Flags={Z=1,N=0,C=0,V=0}
SUB (8-8): A=8, B=8, Result=0, Flags={Z=1,N=0,C=0,V=0}
AND (F&F): A=f, B=f, Result=f, Flags={Z=0,N=1,C=0,V=0}

========================================
      ALU Core Testbench Finished
========================================
```

---

## 8. XDC约束文件

```xdc
################################################################################
# ALU实验约束文件
# 文件名: alu_lab.xdc
# 适用开发板: Digilent Basys3
################################################################################

##------------------------------------------------------------------------------
## 时钟约束
##------------------------------------------------------------------------------
set_property PACKAGE_PIN W5 [get_ports clk]
set_property IOSTANDARD LVCMOS33 [get_ports clk]

##------------------------------------------------------------------------------
## 按钮约束
##------------------------------------------------------------------------------
# 中心按钮 - 复位
set_property PACKAGE_PIN U18 [get_ports btnC]
set_property IOSTANDARD LVCMOS33 [get_ports btnC]

# 上按钮 - 操作码+
set_property PACKAGE_PIN T18 [get_ports btnU]
set_property IOSTANDARD LVCMOS33 [get_ports btnU]

# 下按钮 - 操作码-
set_property PACKAGE_PIN U17 [get_ports btnD]
set_property IOSTANDARD LVCMOS33 [get_ports btnD]

##------------------------------------------------------------------------------
## 拨码开关约束 (SW0-SW15)
##------------------------------------------------------------------------------
set_property PACKAGE_PIN V17 [get_ports {sw[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[0]}]

set_property PACKAGE_PIN V16 [get_ports {sw[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[1]}]

set_property PACKAGE_PIN W16 [get_ports {sw[2]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[2]}]

set_property PACKAGE_PIN W17 [get_ports {sw[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[3]}]

set_property PACKAGE_PIN W15 [get_ports {sw[4]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[4]}]

set_property PACKAGE_PIN V15 [get_ports {sw[5]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[5]}]

set_property PACKAGE_PIN W14 [get_ports {sw[6]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[6]}]

set_property PACKAGE_PIN W13 [get_ports {sw[7]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[7]}]

set_property PACKAGE_PIN V2 [get_ports {sw[8]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[8]}]

set_property PACKAGE_PIN T3 [get_ports {sw[9]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[9]}]

set_property PACKAGE_PIN T2 [get_ports {sw[10]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[10]}]

set_property PACKAGE_PIN R3 [get_ports {sw[11]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[11]}]

set_property PACKAGE_PIN W2 [get_ports {sw[12]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[12]}]

set_property PACKAGE_PIN U1 [get_ports {sw[13]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[13]}]

set_property PACKAGE_PIN T1 [get_ports {sw[14]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[14]}]

set_property PACKAGE_PIN R2 [get_ports {sw[15]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[15]}]

##------------------------------------------------------------------------------
## LED约束 (LD0-LD15)
##------------------------------------------------------------------------------
set_property PACKAGE_PIN U16 [get_ports {led[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[0]}]

set_property PACKAGE_PIN E19 [get_ports {led[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[1]}]

set_property PACKAGE_PIN U19 [get_ports {led[2]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[2]}]

set_property PACKAGE_PIN V19 [get_ports {led[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[3]}]

set_property PACKAGE_PIN W18 [get_ports {led[4]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[4]}]

set_property PACKAGE_PIN U15 [get_ports {led[5]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[5]}]

set_property PACKAGE_PIN U14 [get_ports {led[6]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[6]}]

set_property PACKAGE_PIN V14 [get_ports {led[7]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[7]}]

set_property PACKAGE_PIN V13 [get_ports {led[8]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[8]}]

set_property PACKAGE_PIN V3 [get_ports {led[9]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[9]}]

set_property PACKAGE_PIN W3 [get_ports {led[10]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[10]}]

set_property PACKAGE_PIN U3 [get_ports {led[11]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[11]}]

set_property PACKAGE_PIN P3 [get_ports {led[12]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[12]}]

set_property PACKAGE_PIN N3 [get_ports {led[13]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[13]}]

set_property PACKAGE_PIN P1 [get_ports {led[14]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[14]}]

set_property PACKAGE_PIN L1 [get_ports {led[15]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[15]}]

##------------------------------------------------------------------------------
## 七段数码管约束
##------------------------------------------------------------------------------
# 段选 (CA-CG)
set_property PACKAGE_PIN W7 [get_ports {seg[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {seg[0]}]

set_property PACKAGE_PIN W6 [get_ports {seg[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports {seg[1]}]

set_property PACKAGE_PIN U8 [get_ports {seg[2]}]
set_property IOSTANDARD LVCMOS33 [get_ports {seg[2]}]

set_property PACKAGE_PIN V8 [get_ports {seg[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {seg[3]}]

set_property PACKAGE_PIN U5 [get_ports {seg[4]}]
set_property IOSTANDARD LVCMOS33 [get_ports {seg[4]}]

set_property PACKAGE_PIN V5 [get_ports {seg[5]}]
set_property IOSTANDARD LVCMOS33 [get_ports {seg[5]}]

set_property PACKAGE_PIN U7 [get_ports {seg[6]}]
set_property IOSTANDARD LVCMOS33 [get_ports {seg[6]}]

# 位选 (AN0-AN3)
set_property PACKAGE_PIN U2 [get_ports {an[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {an[0]}]

set_property PACKAGE_PIN U4 [get_ports {an[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports {an[1]}]

set_property PACKAGE_PIN V4 [get_ports {an[2]}]
set_property IOSTANDARD LVCMOS33 [get_ports {an[2]}]

set_property PACKAGE_PIN W4 [get_ports {an[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {an[3]}]

##------------------------------------------------------------------------------
## 配置设置
##------------------------------------------------------------------------------
set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]
set_property CONFIG_VOLTAGE 3.3 [current_design]
set_property CFGBVS VCCO [current_design]
```

---

## 9. 烧录与验证

### 9.1 硬件操作流程

```
┌─────────────────────────────────────────────────────────────┐
│                    ALU实验操作流程                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  步骤1: 设置操作数                                           │
│  ├── 拨动 SW0-SW3 设置操作数A (二进制)                        │
│  └── 拨动 SW4-SW7 设置操作数B (二进制)                        │
│                                                             │
│  步骤2: 选择操作                                             │
│  ├── 按 BTNU 增加操作码                                      │
│  ├── 按 BTND 减少操作码                                      │
│  └── 拨动 SW15 确认操作                                      │
│                                                             │
│  步骤3: 观察结果                                             │
│  ├── 七段数码管显示运算结果                                   │
│  ├── LED0-3 显示操作数A                                      │
│  ├── LED4-7 显示操作数B                                      │
│  ├── LED8-11 显示结果                                        │
│  └── LED12-15 显示标志位                                     │
│                                                             │
│  步骤4: 测试验证                                             │
│  ├── 改变输入验证不同组合                                     │
│  └── 检查标志位是否符合预期                                   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 9.2 功能测试用例

| 测试项 | 操作数A | 操作数B | 操作 | 预期结果 | 预期标志 |
|--------|---------|---------|------|----------|----------|
| 加法 | 5 | 3 | ADD | 8 | N=0,Z=0 |
| 进位 | 10 | 7 | ADD | 1 | C=1 |
| 溢出 | 7 | 7 | ADD | 14 | V=1 |
| 零结果 | 8 | 8 | ADD | 0 | Z=1,C=1 |
| 减法 | 9 | 4 | SUB | 5 | - |
| 借位 | 3 | 7 | SUB | 12 | C=1,N=1 |
| 与 | 1010 | 1100 | AND | 1000 | N=1 |
| 或 | 1010 | 1100 | OR | 1110 | N=1 |
| 异或 | 1010 | 1100 | XOR | 0110 | N=0 |
| 非 | 1010 | - | NOT | 0101 | N=0 |

---

## 10. 故障排除FAQ

### Q1: 运算结果不正确

**问题描述：** ALU计算结果与预期不符

**可能原因及解决方案：**

```
原因1: 操作码译码错误
检查: 确认alu_op信号与操作类型的对应关系
解决: 使用ModelSim仿真验证操作码译码逻辑

原因2: 加减法进位/借位处理错误
检查: 确认减法时是否使用补码（~B + 1）
解决: 参考adder_nbit模块实现

原因3: 标志位计算错误
检查: 确认flag_c在减法时的定义（与加法相反）
解决: flag_c = ~sub_cout 而非 flag_c = sub_cout
```

### Q2: 七段数码管显示异常

**问题描述：** 数码管不显示或显示错误

**解决方案：**

```
检查1: 扫描频率
- 确保扫描频率在1-5KHz范围内
- 频率过低会闪烁，过高会显示暗淡

检查2: 位选信号
- 确认AN信号低电平有效
- 检查扫描逻辑是否正确循环4位

检查3: 译码表
- 确认七段译码表对应共阳极数码管
- 低电平有效（0点亮，1熄灭）
```

### Q3: 按钮控制不灵敏

**问题描述：** 按钮按下无反应或反应多次

**解决方案：**

```
原因: 按键抖动
解决: 确认button_debounce模块已正确实例化

调试方法:
1. 使用ILA捕获按钮信号
2. 检查消抖计数器是否足够大（10ms以上）
3. 确认边沿检测逻辑正确

临时绕过:
- 使用拨码开关代替按钮进行操作码选择
- 在测试阶段简化控制逻辑
```

---

## 11. 实验报告模板

```markdown
    # FPGA实验二报告：ALU实现实验

    ## 基本信息
    - **姓名**: _______________
    - **学号**: _______________
    - **实验日期**: _______________

    ## 一、实验目的
    1. 理解ALU的结构和工作原理
    2. 掌握算术运算和逻辑运算的硬件实现
    3. 理解标志位的产生机制
    4. 学习复杂数字模块的设计方法

    ## 二、ALU设计原理

    ### 2.1 ALU结构框图
    （手绘或绘制ALU结构图）

    ### 2.2 加减法实现
    - 加法器类型: _______________
    - 减法实现方法: _______________

    ### 2.3 标志位逻辑
    | 标志 | 计算公式 | 说明 |
    |------|----------|------|
    | Z | _____________ | _____________ |
    | N | _____________ | _____________ |
    | C | _____________ | _____________ |
    | V | _____________ | _____________ |

    ## 三、核心代码

    ### 3.1 ALU模块
    ```verilog
    // 粘贴关键代码
    ```

    ### 3.2 按钮消抖模块

    ```verilog
    // 粘贴关键代码
    ```

    ## 四、仿真结果

    ### 4.1 仿真波形截图

    （粘贴波形截图）

    ### 4.2 功能验证表

    | 操作 | A | B | 结果 | Z | N | C | V | 是否正确 |
    |------|---|---|------|---|---|---|---|----------|
    | ADD | 5 | 3 | | | | | | |
    | SUB | 9 | 4 | | | | | | |
    | AND | A | C | | | | | | |

    ## 五、硬件验证

    ### 5.1 资源使用

    | 资源类型 | 使用量 | 利用率 |
    |----------|--------|--------|
    | LUT | | |
    | FF | | |
    | IO | | |

    ### 5.2 实测结果

    （记录至少5组测试数据）

    ## 六、问题与解决

    ### 问题1

    **现象**:
    **原因**:
    **解决**:

    ## 七、心得体会

    ## 八、扩展设计（选做）

    描述你实现的扩展功能，如：

    - 8位ALU
    - 乘法器
    - 移位运算

    ## 附录

    - 完整代码
    - XDC文件
    - 仿真报告

```

---

## 12. 扩展练习

### 12.1 8位ALU扩展

将4位ALU扩展为8位，支持更大的数值范围。

### 12.2 乘除法单元

添加乘法和除法运算支持。

### 12.3 移位运算

添加左移、右移、算术右移操作。

### 12.4 流水线ALU

将ALU设计为两级流水线，提高工作频率。

---

## 13. 参考资料

1. 《计算机组成与设计：硬件/软件接口》- David Patterson
2. 《数字逻辑与计算机设计基础》- M. Morris Mano
3. Xilinx Vivado设计套件文档
4. Digilent Basys3参考手册

---

**文档信息**

- 版本: v1.0
- 创建日期: 2024
- 适用课程: 计算机组成原理/FPGA设计


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
