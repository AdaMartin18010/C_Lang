---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
﻿# RV32IM 带缓存处理器设计与实现

## 目录

- [1. M扩展概述](#1-m扩展概述)
  - [1.1 RV32M指令集](#11-rv32m指令集)
  - [1.2 特殊值处理](#12-特殊值处理)
- [2. 乘法器与除法器设计](#2-乘法器与除法器设计)
  - [2.1 乘法器设计](#21-乘法器设计)
    - [2.1.1 基础乘法器 (移位相加)](#211-基础乘法器-移位相加)
    - [2.1.2 Booth编码乘法器](#212-booth编码乘法器)
    - [2.1.3 阵列乘法器 (单周期)](#213-阵列乘法器-单周期)
  - [2.2 除法器设计](#22-除法器设计)
  - [2.3 乘除法单元集成](#23-乘除法单元集成)
- [3. 缓存设计原理](#3-缓存设计原理)
  - [3.1 缓存基础](#31-缓存基础)
  - [3.2 缓存映射方式](#32-缓存映射方式)
  - [3.3 缓存写策略](#33-缓存写策略)
- [4. 指令缓存实现](#4-指令缓存实现)
  - [4.1 I-Cache设计参数](#41-i-cache设计参数)
  - [4.2 I-Cache Verilog实现](#42-i-cache-verilog实现)
- [5. 数据缓存实现](#5-数据缓存实现)
  - [5.1 D-Cache设计参数](#51-d-cache设计参数)
  - [5.2 D-Cache Verilog实现](#52-d-cache-verilog实现)
- [6. 缓存一致性](#6-缓存一致性)
  - [6.1 一致性协议](#61-一致性协议)
- [7. 完整SoC设计](#7-完整soc设计)
  - [7.1 SoC架构图](#71-soc架构图)
  - [7.2 总线接口](#72-总线接口)
- [8. Verilog实现](#8-verilog实现)
  - [8.1 带缓存的完整CPU](#81-带缓存的完整cpu)
  - [8.2 性能比较](#82-性能比较)
- [深入理解](#深入理解)
  - [核心原理](#核心原理)
  - [实践应用](#实践应用)
  - [最佳实践](#最佳实践)

---

## 1. M扩展概述

### 1.1 RV32M指令集

RV32M扩展在RV32I基础上增加了整数乘法和除法指令。

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    RV32M指令列表                                             │
├─────────┬─────────┬─────────────────────────────────────────────────────────┤
│ 指令    │ 格式    │ 操作                                                    │
├─────────┼─────────┼─────────────────────────────────────────────────────────┤
│ MUL     │ R       │ rd = (rs1 × rs2)[31:0]   # 低32位乘法                    │
│ MULH    │ R       │ rd = (rs1 × rs2)[63:32]  # 高32位有符号乘法              │
│ MULHSU  │ R       │ rd = (rs1 × rs2)[63:32]  # 高32位 rs1有符号, rs2无符号   │
│ MULHU   │ R       │ rd = (rs1 × rs2)[63:32]  # 高32位无符号乘法              │
│ DIV     │ R       │ rd = rs1 / rs2           # 有符号除法                    │
│ DIVU    │ R       │ rd = rs1 / rs2           # 无符号除法                    │
│ REM     │ R       │ rd = rs1 % rs2           # 有符号取余                    │
│ REMU    │ R       │ rd = rs1 % rs2           # 无符号取余                    │
└─────────┴─────────┴─────────────────────────────────────────────────────────┘

编码 (R-type):
│31:25    │24:20│19:15│14:12│11:7 │6:0      │
│funct7   │rs2  │rs1  │funct3│rd   │opcode   │

MUL*:    funct7=0000001, funct3=000
MULH:    funct7=0000001, funct3=001
MULHSU:  funct7=0000001, funct3=010
MULHU:   funct7=0000001, funct3=011
DIV*:    funct7=0000001, funct3=100
DIVU:    funct7=0000001, funct3=101
REM*:    funct7=0000001, funct3=110
REMU:    funct7=0000001, funct3=111
```

### 1.2 特殊值处理

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    除法和取余的特殊情况                                       │
└─────────────────────────────────────────────────────────────────────────────┘

除法溢出:
- 有符号: -2^31 / -1 = 2^31 (溢出, 结果为-2^31)
- 无符号: 无溢出情况

除零:
- 所有除法和取余指令, 除数为0时, 结果全为1

取余特殊情况:
┌────────────────┬────────────────┬────────────────┬──────────────────────────┐
│ 被除数         │ 除数           │ 商             │ 余数                     │
├────────────────┼────────────────┼────────────────┼──────────────────────────┤
│ -2^31          │ -1             │ -2^31 (溢出)   │ 0                        │
│ 任意           │ 0              │ -1 (全1)       │ 被除数                   │
│ 6              │ 3              │ 2              │ 0                        │
│ -6             │ 3              │ -2             │ 0                        │
│ 6              │ -3             │ -2             │ 0                        │
│ -6             │ -3             │ 2              │ 0                        │
│ 7              │ 3              │ 2              │ 1                        │
│ -7             │ 3              │ -2             │ -1                       │
└────────────────┴────────────────┴────────────────┴──────────────────────────┘
```

---

## 2. 乘法器与除法器设计

### 2.1 乘法器设计

#### 2.1.1 基础乘法器 (移位相加)

```verilog
// 迭代式乘法器 (多周期)
module multiplier_iterative(
    input         clk,
    input         reset,
    input         start,
    input  [31:0] multiplicand,
    input  [31:0] multiplier,
    input         signed_op,       // 1=有符号, 0=无符号
    output [63:0] product,
    output        done
);
    reg [63:0]  product_reg;
    reg [31:0]  multiplicand_reg;
    reg [5:0]   counter;
    reg         busy;

    // 符号扩展后的被乘数
    wire [63:0] multiplicand_ext = signed_op ?
                                   {{32{multiplicand[31]}}, multiplicand} :
                                   {32'b0, multiplicand};

    // 状态机
    localparam IDLE = 2'b00;
    localparam CALC = 2'b01;
    localparam DONE = 2'b10;

    reg [1:0] state;

    always @(posedge clk or posedge reset) begin
        if (reset) begin
            state <= IDLE;
            product_reg <= 64'b0;
            multiplicand_reg <= 32'b0;
            counter <= 6'b0;
            busy <= 1'b0;
        end else begin
            case (state)
                IDLE: begin
                    if (start) begin
                        product_reg <= {32'b0, multiplier};
                        multiplicand_reg <= multiplicand;
                        counter <= 6'd32;
                        state <= CALC;
                        busy <= 1'b1;
                    end
                end

                CALC: begin
                    if (product_reg[0]) begin
                        product_reg[63:32] <= product_reg[63:32] + multiplicand_reg;
                    end
                    product_reg <= product_reg >> 1;
                    counter <= counter - 1'b1;

                    if (counter == 6'd1) begin
                        state <= DONE;
                    end
                end

                DONE: begin
                    state <= IDLE;
                    busy <= 1'b0;
                end
            endcase
        end
    end

    assign product = product_reg;
    assign done = (state == DONE);
endmodule
```

#### 2.1.2 Booth编码乘法器

```verilog
// Booth算法乘法器 (更快的多周期乘法)
module multiplier_booth(
    input         clk,
    input         reset,
    input         start,
    input  [31:0] multiplicand,
    input  [31:0] multiplier,
    input         signed_op,
    output [63:0] product,
    output        done
);
    reg [63:0]  a_reg;      // 被乘数扩展
    reg [63:0]  s_reg;      // 负被乘数扩展
    reg [63:0]  p_reg;      // 乘积寄存器
    reg [5:0]   counter;
    reg [1:0]   state;

    localparam IDLE = 2'b00;
    localparam CALC = 2'b01;
    localparam DONE = 2'b10;

    // 符号扩展
    wire [63:0] multiplicand_ext = signed_op ?
                                   {{32{multiplicand[31]}}, multiplicand} :
                                   {32'b0, multiplicand};
    wire [63:0] neg_multiplicand = (~multiplicand_ext) + 1'b1;

    always @(posedge clk or posedge reset) begin
        if (reset) begin
            state <= IDLE;
            a_reg <= 64'b0;
            s_reg <= 64'b0;
            p_reg <= 64'b0;
            counter <= 6'b0;
        end else begin
            case (state)
                IDLE: begin
                    if (start) begin
                        a_reg <= multiplicand_ext << 32;
                        s_reg <= neg_multiplicand << 32;
                        p_reg <= {32'b0, multiplier, 1'b0};
                        counter <= 6'd32;
                        state <= CALC;
                    end
                end

                CALC: begin
                    case (p_reg[1:0])
                        2'b01: p_reg <= p_reg + a_reg;
                        2'b10: p_reg <= p_reg + s_reg;
                        default: p_reg <= p_reg;
                    endcase

                    p_reg <= p_reg >>> 1;  // 算术右移
                    counter <= counter - 1'b1;

                    if (counter == 6'd1) begin
                        state <= DONE;
                    end
                end

                DONE: begin
                    state <= IDLE;
                end
            endcase
        end
    end

    assign product = p_reg[64:1];
    assign done = (state == DONE);
endmodule
```

#### 2.1.3 阵列乘法器 (单周期)

```verilog
// 组合逻辑阵列乘法器 (快速但面积大)
module multiplier_array(
    input  [31:0] a,
    input  [31:0] b,
    input         signed_op,
    output [63:0] product
);
    wire [63:0] a_ext = signed_op ? {{32{a[31]}}, a} : {32'b0, a};
    wire [63:0] b_ext = signed_op ? {{32{b[31]}}, b} : {32'b0, b};

    // 使用Verilog内置乘法 (综合器会优化为DSP块或逻辑)
    assign product = $signed(a_ext) * $signed(b_ext);
endmodule
```

### 2.2 除法器设计

```verilog
// 恢复余数除法器 (多周期)
module divider_restoring(
    input         clk,
    input         reset,
    input         start,
    input  [31:0] dividend,
    input  [31:0] divisor,
    input         signed_op,
    output [31:0] quotient,
    output [31:0] remainder,
    output        done
);
    reg [31:0]  quotient_reg;
    reg [63:0]  remainder_reg;
    reg [31:0]  divisor_reg;
    reg [5:0]   counter;
    reg [1:0]   state;
    reg         dividend_neg, divisor_neg;

    localparam IDLE = 2'b00;
    localparam CALC = 2'b01;
    localparam DONE = 2'b10;

    // 绝对值计算
    wire [31:0] abs_dividend = signed_op && dividend[31] ? (~dividend) + 1 : dividend;
    wire [31:0] abs_divisor  = signed_op && divisor[31]  ? (~divisor)  + 1 : divisor;

    always @(posedge clk or posedge reset) begin
        if (reset) begin
            state <= IDLE;
            quotient_reg <= 32'b0;
            remainder_reg <= 64'b0;
            divisor_reg <= 32'b0;
            counter <= 6'b0;
            dividend_neg <= 1'b0;
            divisor_neg <= 1'b0;
        end else begin
            case (state)
                IDLE: begin
                    if (start) begin
                        // 记录符号
                        dividend_neg <= signed_op && dividend[31];
                        divisor_neg <= signed_op && divisor[31];

                        // 使用绝对值进行计算
                        remainder_reg <= {32'b0, abs_dividend};
                        divisor_reg <= abs_divisor;
                        quotient_reg <= 32'b0;
                        counter <= 6'd32;
                        state <= CALC;
                    end
                end

                CALC: begin
                    // 左移余数
                    remainder_reg <= remainder_reg << 1;

                    // 减法
                    if (remainder_reg[63:32] >= divisor_reg) begin
                        remainder_reg[63:32] <= remainder_reg[63:32] - divisor_reg;
                        quotient_reg <= {quotient_reg[30:0], 1'b1};
                    end else begin
                        quotient_reg <= {quotient_reg[30:0], 1'b0};
                    end

                    counter <= counter - 1'b1;

                    if (counter == 6'd1) begin
                        state <= DONE;
                    end
                end

                DONE: begin
                    state <= IDLE;
                end
            endcase
        end
    end

    // 结果符号调整
    wire [31:0] quotient_abs = quotient_reg;
    wire [31:0] remainder_abs = remainder_reg[63:32];

    wire [31:0] quotient_final = signed_op && (dividend_neg ^ divisor_neg) ?
                                  (~quotient_abs) + 1 : quotient_abs;
    wire [31:0] remainder_final = signed_op && dividend_neg ?
                                   (~remainder_abs) + 1 : remainder_abs;

    assign quotient = quotient_final;
    assign remainder = remainder_final;
    assign done = (state == DONE);
endmodule
```

### 2.3 乘除法单元集成

```verilog
// 乘除法单元 (MDU) - 集成到流水线中
module mul_div_unit(
    input         clk,
    input         reset,
    input         en,              // 使能
    input  [3:0]  op,              // 操作码
    input  [31:0] rs1,
    input  [31:0] rs2,
    output [31:0] result,
    output        busy,            // 正在计算
    output        done             // 计算完成
);
    // 操作码定义
    localparam OP_MUL    = 4'b0000;
    localparam OP_MULH   = 4'b0001;
    localparam OP_MULHSU = 4'b0010;
    localparam OP_MULHU  = 4'b0011;
    localparam OP_DIV    = 4'b0100;
    localparam OP_DIVU   = 4'b0101;
    localparam OP_REM    = 4'b0110;
    localparam OP_REMU   = 4'b0111;

    // 乘除法器实例
    wire [63:0] mul_result;
    wire        mul_done;

    multiplier_iterative mul_unit(
        .clk(clk),
        .reset(reset),
        .start(en && (op[2] == 1'b0)),
        .multiplicand(rs1),
        .multiplier(rs2),
        .signed_op(~op[0]),
        .product(mul_result),
        .done(mul_done)
    );

    wire [31:0] div_quotient, div_remainder;
    wire        div_done;

    divider_restoring div_unit(
        .clk(clk),
        .reset(reset),
        .start(en && (op[2] == 1'b1)),
        .dividend(rs1),
        .divisor(rs2),
        .signed_op(~op[0]),
        .quotient(div_quotient),
        .remainder(div_remainder),
        .done(div_done)
    );

    // 结果选择
    reg [31:0] result_reg;
    reg        done_reg;

    always @(posedge clk or posedge reset) begin
        if (reset) begin
            result_reg <= 32'b0;
            done_reg <= 1'b0;
        end else if (en) begin
            case (op)
                OP_MUL:    result_reg <= mul_result[31:0];
                OP_MULH:   result_reg <= mul_result[63:32];
                OP_MULHSU: result_reg <= mul_result[63:32];
                OP_MULHU:  result_reg <= mul_result[63:32];
                OP_DIV:    result_reg <= div_quotient;
                OP_DIVU:   result_reg <= div_quotient;
                OP_REM:    result_reg <= div_remainder;
                OP_REMU:   result_reg <= div_remainder;
                default:   result_reg <= 32'b0;
            endcase
            done_reg <= mul_done || div_done;
        end else begin
            done_reg <= 1'b0;
        end
    end

    assign result = result_reg;
    assign done = done_reg;
    assign busy = en && ~done_reg;
endmodule
```

---

## 3. 缓存设计原理

### 3.1 缓存基础

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    缓存层次结构                                              │
└─────────────────────────────────────────────────────────────────────────────┘

处理器核心
     │
     ▼
┌─────────────┐  最快, 最小, 最昂贵
│  L1 I-Cache │  通常 16-64KB
│  L1 D-Cache │  通常 16-64KB
├─────────────┤
│   L2 Cache  │  较快, 较大
│   (Unified) │  通常 256KB-1MB
├─────────────┤
│   L3 Cache  │  较慢, 最大
│   (Shared)  │  通常 4MB-64MB
├─────────────┤
│  Main Memory│  最慢, 最大, 最便宜
│   (DRAM)    │  通常 4GB-1TB
└─────────────┘

缓存设计目标:
1. 减少平均内存访问时间 (AMAT)
2. 提高命中率
3. 降低未命中惩罚
```

### 3.2 缓存映射方式

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    三种缓存映射方式                                          │
└─────────────────────────────────────────────────────────────────────────────┘

1. 直接映射 (Direct Mapped)
─────────────────────────────────────────────────────────────────────────────
地址划分: [Tag | Index | Offset]

内存块只能放入缓存的特定行
Index = Memory_Block_Address % Number_of_Cache_Lines

优点: 简单, 快速, 无需替换策略
缺点: 冲突率高

示例: 4行直接映射缓存
Memory Block 0 → Cache Line 0
Memory Block 1 → Cache Line 1
Memory Block 2 → Cache Line 2
Memory Block 3 → Cache Line 3
Memory Block 4 → Cache Line 0 (冲突!)
Memory Block 5 → Cache Line 1 (冲突!)


2. 全相联 (Fully Associative)
─────────────────────────────────────────────────────────────────────────────
地址划分: [Tag | Offset]

内存块可以放入缓存的任意行

优点: 冲突率最低
缺点: 需要并行比较所有Tag, 硬件复杂


3. 组相联 (Set Associative)
─────────────────────────────────────────────────────────────────────────────
地址划分: [Tag | Index | Offset]

N路组相联: 缓存分为若干组, 每组N行
内存块映射到特定组, 组内任意行可存放

优点: 平衡了冲突率和硬件复杂度
常见配置: 2-way, 4-way, 8-way

示例: 4路组相联, 总共16行 → 4组, 每组4行
Memory Block 0 → Set 0 (可选 Line 0,1,2,3)
Memory Block 1 → Set 1 (可选 Line 4,5,6,7)
Memory Block 4 → Set 0 (可选 Line 0,1,2,3)
```

### 3.3 缓存写策略

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    缓存写策略                                                │
└─────────────────────────────────────────────────────────────────────────────┘

写命中策略:

1. 写直达 (Write-Through)
   - 同时更新缓存和内存
   - 优点: 数据一致性高
   - 缺点: 每次写都要访问内存, 速度慢

2. 写回 (Write-Back)
   - 只更新缓存, 被替换时才写回内存
   - 需要脏位(Dirty Bit)标记
   - 优点: 减少内存访问
   - 缺点: 数据一致性复杂

写未命中策略:

1. 写分配 (Write-Allocate)
   - 先加载数据到缓存, 再写
   - 通常与写回配合使用

2. 非写分配 (No-Write-Allocate)
   - 直接写内存, 不加载到缓存
   - 通常与写直达配合使用
```

---

## 4. 指令缓存实现

### 4.1 I-Cache设计参数

```
设计规格:
- 容量: 4KB
- 行大小: 32字节 (8条指令)
- 组织: 2路组相联
- 替换策略: LRU
- 策略: 只读, 写直达 (实际上不修改)

参数计算:
- 总行数 = 4KB / 32B = 128行
- 组数 = 128 / 2 = 64组
- Index位数 = log2(64) = 6位
- Offset位数 = log2(32) = 5位
- Tag位数 = 32 - 6 - 5 = 21位
```

### 4.2 I-Cache Verilog实现

```verilog
//////////////////////////////////////////////////////////////////////////////
// 指令缓存 (I-Cache)
// 规格: 4KB, 2-way Set Associative, 32B Line Size
//////////////////////////////////////////////////////////////////////////////

module icache(
    input         clk,
    input         reset,
    // 处理器接口
    input  [31:0] cpu_addr,
    input         cpu_req,
    output [31:0] cpu_rdata,
    output        cpu_ready,
    // 内存接口
    output [31:0] mem_addr,
    output        mem_req,
    input  [255:0] mem_rdata,      // 256bit = 32B line
    input         mem_ready
);
    // 缓存参数
    localparam LINE_SIZE    = 32;        // 32 bytes
    localparam NUM_WAYS     = 2;         // 2-way
    localparam NUM_SETS     = 64;        // 64 sets
    localparam TAG_WIDTH    = 21;        // 32 - 6 - 5
    localparam INDEX_WIDTH  = 6;
    localparam OFFSET_WIDTH = 5;

    // 地址分解
    wire [OFFSET_WIDTH-1:0] offset  = cpu_addr[OFFSET_WIDTH-1:0];
    wire [INDEX_WIDTH-1:0]  index   = cpu_addr[INDEX_WIDTH+OFFSET_WIDTH-1:OFFSET_WIDTH];
    wire [TAG_WIDTH-1:0]    tag     = cpu_addr[31:INDEX_WIDTH+OFFSET_WIDTH];

    // 缓存存储
    // Tag数组
    reg [TAG_WIDTH-1:0]  tag_array  [0:NUM_SETS-1][0:NUM_WAYS-1];
    // 有效位
    reg                  valid_array[0:NUM_SETS-1][0:NUM_WAYS-1];
    // LRU位 (每组1位, 0=way0, 1=way1)
    reg                  lru_array  [0:NUM_SETS-1];
    // 数据存储 (使用块RAM)
    reg [255:0]          data_array [0:NUM_SETS-1][0:NUM_WAYS-1];

    // 状态机
    localparam IDLE   = 2'b00;
    localparam LOOKUP = 2'b01;
    localparam REFILL = 2'b10;

    reg [1:0] state;

    // 查找结果
    wire [TAG_WIDTH-1:0] tag_way0 = tag_array[index][0];
    wire [TAG_WIDTH-1:0] tag_way1 = tag_array[index][1];
    wire valid_way0 = valid_array[index][0];
    wire valid_way1 = valid_array[index][1];

    // 命中检测
    wire hit_way0 = valid_way0 && (tag_way0 == tag);
    wire hit_way1 = valid_way1 && (tag_way1 == tag);
    wire cache_hit = hit_way0 || hit_way1;
    wire hit_way = hit_way1;  // 1=way1, 0=way0

    // 数据选择
    wire [255:0] line_data = hit_way0 ? data_array[index][0] : data_array[index][1];

    // 从32B line中选择字
    wire [4:0] word_offset = offset[4:2];  // 哪个字 (0-7)
    reg [31:0] selected_word;

    always @(*) begin
        case (word_offset)
            3'd0: selected_word = line_data[31:0];
            3'd1: selected_word = line_data[63:32];
            3'd2: selected_word = line_data[95:64];
            3'd3: selected_word = line_data[127:96];
            3'd4: selected_word = line_data[159:128];
            3'd5: selected_word = line_data[191:160];
            3'd6: selected_word = line_data[223:192];
            3'd7: selected_word = line_data[255:224];
            default: selected_word = 32'b0;
        endcase
    end

    // 替换选择 (LRU)
    wire lru_way = lru_array[index];
    wire replace_way = (valid_way0 && valid_way1) ? lru_way :
                       (valid_way0 ? 1'b1 : 1'b0);

    // 状态机
    reg [31:0] miss_addr;

    always @(posedge clk or posedge reset) begin
        if (reset) begin
            state <= IDLE;
            miss_addr <= 32'b0;
            // 初始化有效位
            integer i, j;
            for (i = 0; i < NUM_SETS; i = i + 1) begin
                lru_array[i] <= 1'b0;
                for (j = 0; j < NUM_WAYS; j = j + 1) begin
                    valid_array[i][j] <= 1'b0;
                end
            end
        end else begin
            case (state)
                IDLE: begin
                    if (cpu_req) begin
                        state <= LOOKUP;
                    end
                end

                LOOKUP: begin
                    if (cache_hit) begin
                        // 更新LRU
                        lru_array[index] <= hit_way;
                        state <= IDLE;
                    end else begin
                        // 未命中, 开始refill
                        miss_addr <= {cpu_addr[31:5], 5'b0};
                        state <= REFILL;
                    end
                end

                REFILL: begin
                    if (mem_ready) begin
                        // 存储新行
                        data_array[index][replace_way] <= mem_rdata;
                        tag_array[index][replace_way] <= tag;
                        valid_array[index][replace_way] <= 1'b1;
                        lru_array[index] <= ~replace_way;
                        state <= LOOKUP;
                    end
                end
            endcase
        end
    end

    // 输出
    assign cpu_rdata = selected_word;
    assign cpu_ready = (state == LOOKUP) && cache_hit;

    assign mem_addr = miss_addr;
    assign mem_req = (state == REFILL);

endmodule
```

---

## 5. 数据缓存实现

### 5.1 D-Cache设计参数

```
设计规格:
- 容量: 4KB
- 行大小: 32字节
- 组织: 4路组相联
- 替换策略: Pseudo-LRU
- 写策略: 写回(Write-Back) + 写分配

参数计算:
- 总行数 = 4KB / 32B = 128行
- 组数 = 128 / 4 = 32组
- Index位数 = log2(32) = 5位
- Offset位数 = log2(32) = 5位
- Tag位数 = 32 - 5 - 5 = 22位

每行额外存储:
- Valid bit: 1 bit
- Dirty bit: 1 bit
- Tag: 22 bits
```

### 5.2 D-Cache Verilog实现

```verilog
//////////////////////////////////////////////////////////////////////////////
// 数据缓存 (D-Cache)
// 规格: 4KB, 4-way Set Associative, Write-Back, 32B Line
//////////////////////////////////////////////////////////////////////////////

module dcache(
    input         clk,
    input         reset,
    // 处理器接口
    input  [31:0] cpu_addr,
    input  [31:0] cpu_wdata,
    input         cpu_req,
    input         cpu_we,          // 写使能
    input  [3:0]  cpu_be,          // 字节使能
    output [31:0] cpu_rdata,
    output        cpu_ready,
    // 内存接口
    output [31:0] mem_addr,
    output [255:0] mem_wdata,
    input  [255:0] mem_rdata,
    output        mem_req,
    output        mem_we,
    input         mem_ready
);
    // 缓存参数
    localparam LINE_SIZE    = 32;
    localparam NUM_WAYS     = 4;
    localparam NUM_SETS     = 32;
    localparam TAG_WIDTH    = 22;
    localparam INDEX_WIDTH  = 5;
    localparam OFFSET_WIDTH = 5;

    // 地址分解
    wire [OFFSET_WIDTH-1:0] offset = cpu_addr[OFFSET_WIDTH-1:0];
    wire [INDEX_WIDTH-1:0]  index  = cpu_addr[INDEX_WIDTH+OFFSET_WIDTH-1:OFFSET_WIDTH];
    wire [TAG_WIDTH-1:0]    tag    = cpu_addr[31:INDEX_WIDTH+OFFSET_WIDTH];

    // 缓存存储
    reg [TAG_WIDTH-1:0]  tag_array  [0:NUM_SETS-1][0:NUM_WAYS-1];
    reg                  valid_array[0:NUM_SETS-1][0:NUM_WAYS-1];
    reg                  dirty_array[0:NUM_SETS-1][0:NUM_WAYS-1];
    reg [2:0]            plru_array [0:NUM_SETS-1];  // Pseudo-LRU
    reg [255:0]          data_array [0:NUM_SETS-1][0:NUM_WAYS-1];

    // 状态机
    localparam IDLE     = 3'b000;
    localparam LOOKUP   = 3'b001;
    localparam EVICT    = 3'b010;
    localparam REFILL   = 3'b011;
    localparam WRITE    = 3'b100;

    reg [2:0] state;

    // 查找逻辑
    wire [NUM_WAYS-1:0] hit_ways;
    wire cache_hit;
    wire [1:0] hit_way_num;

    genvar w;
    generate
        for (w = 0; w < NUM_WAYS; w = w + 1) begin : hit_detect
            assign hit_ways[w] = valid_array[index][w] &&
                                 (tag_array[index][w] == tag);
        end
    endgenerate

    assign cache_hit = |hit_ways;

    // 编码hit way
    assign hit_way_num = hit_ways[0] ? 2'd0 :
                         hit_ways[1] ? 2'd1 :
                         hit_ways[2] ? 2'd2 : 2'd3;

    // 替换逻辑 (Pseudo-LRU)
    reg [1:0] replace_way;

    always @(*) begin
        if (!valid_array[index][0]) replace_way = 2'd0;
        else if (!valid_array[index][1]) replace_way = 2'd1;
        else if (!valid_array[index][2]) replace_way = 2'd2;
        else if (!valid_array[index][3]) replace_way = 2'd3;
        else begin
            // Pseudo-LRU替换
            if (!plru_array[index][0]) begin
                replace_way = plru_array[index][1] ? 2'd0 : 2'd1;
            end else begin
                replace_way = plru_array[index][2] ? 2'd2 : 2'd3;
            end
        end
    end

    // 数据选择和修改
    reg [255:0] line_data;
    reg [255:0] modified_line;
    reg [31:0]  word_data;

    always @(*) begin
        line_data = data_array[index][state == IDLE ? hit_way_num : replace_way];

        // 字节写修改
        case (offset[4:2])
            3'd0: modified_line = {line_data[255:32], cpu_wdata};
            3'd1: modified_line = {line_data[255:64], cpu_wdata, line_data[31:0]};
            3'd2: modified_line = {line_data[255:96], cpu_wdata, line_data[63:0]};
            3'd3: modified_line = {line_data[255:128], cpu_wdata, line_data[95:0]};
            3'd4: modified_line = {line_data[255:160], cpu_wdata, line_data[127:0]};
            3'd5: modified_line = {line_data[255:192], cpu_wdata, line_data[159:0]};
            3'd6: modified_line = {line_data[255:224], cpu_wdata, line_data[191:0]};
            3'd7: modified_line = {cpu_wdata, line_data[223:0]};
        endcase
    end

    // 读数据选择
    always @(*) begin
        case (offset[4:2])
            3'd0: word_data = line_data[31:0];
            3'd1: word_data = line_data[63:32];
            3'd2: word_data = line_data[95:64];
            3'd3: word_data = line_data[127:96];
            3'd4: word_data = line_data[159:128];
            3'd5: word_data = line_data[191:160];
            3'd6: word_data = line_data[223:192];
            3'd7: word_data = line_data[255:224];
        endcase
    end

    // 状态机
    reg [31:0] miss_addr;
    reg [255:0] evict_data;
    reg evict_dirty;

    always @(posedge clk or posedge reset) begin
        if (reset) begin
            state <= IDLE;
            miss_addr <= 32'b0;
            evict_data <= 256'b0;
            evict_dirty <= 1'b0;
            integer i, j;
            for (i = 0; i < NUM_SETS; i = i + 1) begin
                plru_array[i] <= 3'b0;
                for (j = 0; j < NUM_WAYS; j = j + 1) begin
                    valid_array[i][j] <= 1'b0;
                    dirty_array[i][j] <= 1'b0;
                end
            end
        end else begin
            case (state)
                IDLE: begin
                    if (cpu_req) begin
                        state <= LOOKUP;
                    end
                end

                LOOKUP: begin
                    if (cache_hit) begin
                        // 更新PLRU
                        plru_array[index][0] <= (hit_way_num >= 2);
                        if (hit_way_num >= 2) begin
                            plru_array[index][2] <= (hit_way_num == 3);
                        end else begin
                            plru_array[index][1] <= (hit_way_num == 1);
                        end

                        if (cpu_we) begin
                            // 写命中
                            data_array[index][hit_way_num] <= modified_line;
                            dirty_array[index][hit_way_num] <= 1'b1;
                        end
                        state <= IDLE;
                    end else begin
                        // 未命中
                        miss_addr <= {cpu_addr[31:5], 5'b0};
                        evict_dirty <= dirty_array[index][replace_way];
                        evict_data <= data_array[index][replace_way];

                        if (dirty_array[index][replace_way]) begin
                            state <= EVICT;
                        end else begin
                            state <= REFILL;
                        end
                    end
                end

                EVICT: begin
                    if (mem_ready) begin
                        state <= REFILL;
                    end
                end

                REFILL: begin
                    if (mem_ready) begin
                        data_array[index][replace_way] <= mem_rdata;
                        tag_array[index][replace_way] <= tag;
                        valid_array[index][replace_way] <= 1'b1;
                        dirty_array[index][replace_way] <= cpu_we;

                        if (cpu_we) begin
                            // 合并写
                            line_data <= mem_rdata;
                            state <= WRITE;
                        end else begin
                            state <= LOOKUP;
                        end
                    end
                end

                WRITE: begin
                    data_array[index][replace_way] <= modified_line;
                    state <= IDLE;
                end
            endcase
        end
    end

    // 输出
    assign cpu_rdata = word_data;
    assign cpu_ready = (state == LOOKUP) && cache_hit;

    assign mem_addr = (state == EVICT) ?
                      {tag_array[index][replace_way], index, 5'b0} : miss_addr;
    assign mem_wdata = evict_data;
    assign mem_req = (state == EVICT) || (state == REFILL);
    assign mem_we = (state == EVICT);

endmodule
```

---

## 6. 缓存一致性

### 6.1 一致性协议

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    MESI缓存一致性协议                                        │
└─────────────────────────────────────────────────────────────────────────────┘

状态:
- M (Modified): 缓存行已被修改, 与内存不一致, 只有该缓存拥有该数据
- E (Exclusive): 缓存行有效, 与内存一致, 只有该缓存拥有该数据
- S (Shared): 缓存行有效, 与内存一致, 可能有其他缓存也拥有该数据
- I (Invalid): 缓存行无效

状态转换:

当前状态    本地读     本地写     远程读     远程写
────────────────────────────────────────────────────────
I           E/S        M          -          -
E           E          M          S          I
S           S          M          S          I
M           M          M          S/O        I

监听总线:
- 每个缓存控制器监听总线上的事务
- 根据事务类型和地址更新本地状态

RISC-V简化方案:
- 单核处理器不需要缓存一致性协议
- 多核使用目录式或监听式协议
- 可选: 硬件I-cache/D-cache一致性
```

---

## 7. 完整SoC设计

### 7.1 SoC架构图

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                              RV32IM SoC 架构图                                       │
└─────────────────────────────────────────────────────────────────────────────────────┘

                              ┌───────────────────┐
                              │   RISC-V Core     │
                              │   (RV32IM + Cache)│
                              │                   │
                              │  ┌─────────────┐  │
                              │  │  Pipeline   │  │
                              │  │  (5-stage)  │  │
                              │  └──────┬──────┘  │
                              │         │         │
                              │  ┌──────▼──────┐  │
                              │  │  I-Cache    │  │
                              │  │  (4KB, 2way)│  │
                              │  └──────┬──────┘  │
                              │         │         │
                              │  ┌──────▼──────┐  │
                              │  │  D-Cache    │  │
                              │  │  (4KB, 4way)│  │
                              │  └─────────────┘  │
                              └─────────┬─────────┘
                                        │
                    ┌───────────────────┼───────────────────┐
                    │                   │                   │
                    ▼                   ▼                   ▼
           ┌────────────────┐  ┌────────────────┐  ┌────────────────┐
           │  Boot ROM      │  │  SRAM          │  │  Peripherals   │
           │  (4KB)         │  │  (64KB)        │  │                │
           │                │  │                │  │  ┌──────────┐  │
           │  0x0000_0000   │  │  0x2000_0000   │  │  │ UART     │  │
           │  启动代码       │  │  数据内存       │  │  │ 0x1000_0000│  │
           └────────────────┘  └────────────────┘  │  └──────────┘  │
                                                   │  ┌──────────┐  │
                                                   │  │ Timer    │  │
                                                   │  │ 0x1000_1000│  │
                                                   │  └──────────┘  │
                                                   │  ┌──────────┐  │
                                                   │  │ GPIO     │  │
                                                   │  │ 0x1000_2000│  │
                                                   │  └──────────┘  │
                                                   └────────────────┘

地址映射:
0x0000_0000 - 0x0000_0FFF: Boot ROM (4KB)
0x0001_0000 - 0x0001_FFFF: I-Cache (直连)
0x0002_0000 - 0x0002_FFFF: D-Cache (直连)
0x1000_0000 - 0x1000_0FFF: UART
0x1000_1000 - 0x1000_1FFF: Timer
0x1000_2000 - 0x1000_2FFF: GPIO
0x2000_0000 - 0x2000_FFFF: SRAM (64KB)
```

### 7.2 总线接口

```verilog
// AHB-Lite总线接口
module ahb_lite_interface(
    input         HCLK,
    input         HRESETn,
    // 主设备接口 (CPU)
    input  [31:0] cpu_addr,
    input  [31:0] cpu_wdata,
    input         cpu_req,
    input         cpu_we,
    input  [2:0]  cpu_size,
    output [31:0] cpu_rdata,
    output        cpu_ready,
    // AHB总线信号
    output [31:0] HADDR,
    output [31:0] HWDATA,
    output        HWRITE,
    output [2:0]  HSIZE,
    output [1:0]  HTRANS,
    output        HSEL,
    input  [31:0] HRDATA,
    input         HREADY
);
    assign HADDR  = cpu_addr;
    assign HWDATA = cpu_wdata;
    assign HWRITE = cpu_we;
    assign HSIZE  = cpu_size;
    assign HTRANS = cpu_req ? 2'b10 : 2'b00;  // NONSEQ or IDLE
    assign HSEL   = cpu_req;

    assign cpu_rdata = HRDATA;
    assign cpu_ready = HREADY;
endmodule
```

---

## 8. Verilog实现

### 8.1 带缓存的完整CPU

```verilog
//////////////////////////////////////////////////////////////////////////////
// RV32IM 带缓存处理器完整实现
//////////////////////////////////////////////////////////////////////////////

module rv32im_cache_cpu(
    input         clk,
    input         reset,
    // 调试输出
    output [31:0] pc_current,
    output [31:0] instruction,
    output [31:0] reg_x1,
    output        cache_miss
);
    // 内部信号
    wire [31:0] icache_rdata;
    wire        icache_ready;
    wire [31:0] icache_addr;
    wire        icache_req;

    wire [31:0] dcache_rdata;
    wire        dcache_ready;
    wire [31:0] dcache_wdata;
    wire [31:0] dcache_addr;
    wire        dcache_req;
    wire        dcache_we;

    wire [31:0] mem_rdata;
    wire [255:0] mem_line_rdata;
    wire        mem_ready;
    wire [31:0] mem_addr;
    wire [255:0] mem_wdata;
    wire        mem_req;
    wire        mem_we;

    // 指令缓存
    icache icache_inst(
        .clk(clk),
        .reset(reset),
        .cpu_addr(icache_addr),
        .cpu_req(icache_req),
        .cpu_rdata(icache_rdata),
        .cpu_ready(icache_ready),
        .mem_addr(mem_addr),
        .mem_req(mem_req),
        .mem_rdata(mem_line_rdata),
        .mem_ready(mem_ready)
    );

    // 数据缓存
    dcache dcache_inst(
        .clk(clk),
        .reset(reset),
        .cpu_addr(dcache_addr),
        .cpu_wdata(dcache_wdata),
        .cpu_req(dcache_req),
        .cpu_we(dcache_we),
        .cpu_be(4'b1111),
        .cpu_rdata(dcache_rdata),
        .cpu_ready(dcache_ready),
        .mem_addr(),
        .mem_wdata(mem_wdata),
        .mem_req(),
        .mem_we(mem_we),
        .mem_rdata(mem_line_rdata),
        .mem_ready(mem_ready)
    );

    // 简化内存 (用于测试)
    memory_controller mem_ctrl(
        .clk(clk),
        .reset(reset),
        .addr(mem_addr),
        .wdata(mem_wdata),
        .req(mem_req),
        .we(mem_we),
        .rdata(mem_line_rdata),
        .ready(mem_ready)
    );

    // 流水线CPU核心 (使用缓存接口)
    rv32i_pipelined_cached core(
        .clk(clk),
        .reset(reset),
        .icache_rdata(icache_rdata),
        .icache_ready(icache_ready),
        .icache_addr(icache_addr),
        .icache_req(icache_req),
        .dcache_rdata(dcache_rdata),
        .dcache_ready(dcache_ready),
        .dcache_wdata(dcache_wdata),
        .dcache_addr(dcache_addr),
        .dcache_req(dcache_req),
        .dcache_we(dcache_we),
        .pc_current(pc_current),
        .instruction(instruction),
        .reg_x1(reg_x1)
    );

    assign cache_miss = !icache_ready && icache_req;

endmodule

// 内存控制器 (模拟)
module memory_controller(
    input         clk,
    input         reset,
    input  [31:0] addr,
    input  [255:0] wdata,
    input         req,
    input         we,
    output [255:0] rdata,
    output        ready
);
    reg [7:0] mem [0:65535];  // 64KB内存
    reg [2:0] counter;
    reg busy;

    // 读取逻辑
    assign rdata = {mem[addr+31], mem[addr+30], ..., mem[addr]};  // 简化
    assign ready = (counter == 3'd0) && busy;

    always @(posedge clk or posedge reset) begin
        if (reset) begin
            counter <= 3'd0;
            busy <= 1'b0;
        end else if (req && !busy) begin
            busy <= 1'b1;
            counter <= 3'd4;  // 4周期延迟模拟
        end else if (busy) begin
            if (counter > 0)
                counter <= counter - 1'b1;
            else
                busy <= 1'b0;
        end
    end
endmodule
```

### 8.2 性能比较

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    缓存对性能的影响                                          │
└─────────────────────────────────────────────────────────────────────────────┘

配置:
- 处理器: 5级流水线 RV32IM
- 时钟频率: 100MHz
- 内存延迟: 10周期 (100ns)
- 缓存命中时间: 1周期 (10ns)
- I-Cache: 4KB, 2-way, 命中率 95%
- D-Cache: 4KB, 4-way, 命中率 90%

性能计算:

AMAT (Average Memory Access Time) = Hit Time + Miss Rate × Miss Penalty

指令访问AMAT = 1 + 0.05 × 10 = 1.5 周期
数据访问AMAT = 1 + 0.10 × 10 = 2.0 周期

假设:
- Load/Store占 30%
- CPI_base = 1.2 (考虑冒险)

CPI_with_cache = 1.2 + (1.5-1) + 0.3×(2.0-1) = 1.2 + 0.5 + 0.3 = 2.0
CPI_no_cache = 1.2 + (10-1) + 0.3×(10-1) = 1.2 + 9 + 2.7 = 12.9

加速比 = 12.9 / 2.0 ≈ 6.45x
```

---

**文档结束**


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
