---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../../01_Core_Knowledge_System/05_Engineering_Layer/readme.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../../02_Formal_Semantics_and_Physics/readme.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../../03_System_Technology_Domains/readme.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../../04_Industrial_Scenarios/readme.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../../06_Thinking_Representation/readme.md) | 核心关联 | 思维表征基础 |
﻿# 乘除法器 (Multiplier & Divider)

## 目录
1. [概述](#概述)
2. [移位乘法器](#移位乘法器)
3. [阵列乘法器](#阵列乘法器)
4. [Booth算法](#booth算法)
5. [恢复余数除法器](#恢复余数除法器)
6. [非恢复余数除法器](#非恢复余数除法器)
7. [性能对比](#性能对比)
8. [与C语言的关系](#与c语言的关系)

---

## 概述

乘除法是计算机中最复杂的基本算术运算。与加法不同，乘除法需要多个时钟周期或复杂的组合逻辑来完成。理解这些运算的硬件实现对于优化数值计算程序至关重要。

### 乘法基础

```
二进制乘法示例 (4位 × 4位):

     1 0 1 1  (被乘数 = 11)
   × 1 1 0 1  (乘数 = 13)
   ─────────
     1 0 1 1  (1 × 被乘数)
   0 0 0 0    (0 × 被乘数，左移1位)
  1 0 1 1     (1 × 被乘数，左移2位)
1 0 1 1       (1 × 被乘数，左移3位)
─────────────────────────────
1 0 0 0 1 1 1 1  (结果 = 143)
```

### 除法基础

```
二进制除法示例 (8位 ÷ 4位):

     1 0 0 0 1  (商 = 17)
   ┌───────────────
1 1 0 1 ) 1 1 1 0 1 0 0 1  (被除数 = 233)
         1 1 0 1
         ───────
           1 0 0 1 0
             1 1 0 1
             ───────
               1 0 0 1 1
                 1 1 0 1
                 ───────
                   0 1 1 0  (余数 = 6)
```

### 运算复杂度对比

```
┌─────────────────────────────────────────────────────────────┐
│                    算术运算复杂度对比                        │
├─────────────────────────────────────────────────────────────┤
│  运算        │  时间复杂度    │  空间复杂度    │  延迟      │
├──────────────┼────────────────┼────────────────┼────────────┤
│  加法        │  O(log n)      │  O(n)          │  1-3周期   │
│  减法        │  O(log n)      │  O(n)          │  1-3周期   │
│  乘法        │  O(n)          │  O(n²)         │  n-2n周期  │
│  除法        │  O(n)          │  O(n)          │  n-2n周期  │
├──────────────┴────────────────┴────────────────┴────────────┤
│  n为操作数位数                                              │
└─────────────────────────────────────────────────────────────┘
```

---

## 移位乘法器

### 原理

移位乘法器（Shift-and-Add Multiplier）是最简单的乘法器实现，通过迭代地将部分积移位并累加来完成乘法运算。

### 算法流程

```
初始化:
  被乘数 (Multiplicand) = M
  乘数 (Multiplier) = Q
  积 (Product) = 0
  计数器 = n (位数)

算法:
  while 计数器 > 0:
    if Q的最低位 == 1:
      积 = 积 + 被乘数
    被乘数左移1位
    乘数右移1位
    计数器 = 计数器 - 1
```

### 数据通路图

```
    ┌─────────────────────────────────────────────────────────────────┐
    │                    移位乘法器数据通路                            │
    │                                                                 │
    │   ┌─────────────┐          ┌─────────────┐                     │
    │   │  被乘数寄存器 │          │   乘数寄存器  │                     │
    │   │   (2n位)    │          │   (n位)      │                     │
    │   │             │          │             │                     │
    │   │ ┌─────────┐ │          │ ┌─────────┐ │                     │
    │   │ │00001011 │ │          │ │  1101   │ │                     │
    │   │ └─────────┘ │          │ └─────────┘ │                     │
    │   │      │      │          │      │      │                     │
    │   │      │      │          │      │ LSB  │                     │
    │   │      ▼      │          │      ▼      │                     │
    │   │  ┌───────┐  │          │  ┌───────┐  │                     │
    │   │  │ 左移  │  │          │  │ 右移  │  │                     │
    │   │  │  1位  │  │          │  │  1位  │  │                     │
    │   │  └───┬───┘  │          │  └───┬───┘  │                     │
    │   │      │      │          │      │      │                     │
    │   └──────┼──────┘          └──────┴──────┘                     │
    │          │                      │                              │
    │          │                      ▼                              │
    │          │                 ┌─────────┐                         │
    │          │                 │ 控制器   │                         │
    │          │                 │  (FSM)  │                         │
    │          │                 └────┬────┘                         │
    │          │                      │                              │
    │          ▼                      │                              │
    │   ┌─────────────┐               │ 控制信号                      │
    │   │   加法器    │◄──────────────┘                              │
    │   │  (2n位)    │                                               │
    │   └──────┬──────┘                                               │
    │          │                                                      │
    │          ▼                                                      │
    │   ┌─────────────┐                                               │
    │   │   积寄存器   │                                               │
    │   │   (2n位)    │                                               │
    │   │             │                                               │
    │   │ ┌─────────┐ │                                               │
    │   │ │00000000 │ │                                               │
    │   │ └─────────┘ │                                               │
    │   └─────────────┘                                               │
    │                                                                 │
    └─────────────────────────────────────────────────────────────────┘
```

### 详细运算过程示例

```
4位乘法: 1011 × 1101 (11 × 13 = 143)

时钟周期0 (初始化):
  被乘数: 0000 1011
  乘数:       1101
  积:   0000 0000 0000

时钟周期1 (乘数LSB=1):
  积 = 积 + 被乘数 = 0000 0000 + 0000 1011 = 0000 1011
  被乘数左移: 0001 0110
  乘数右移:       0110

时钟周期2 (乘数LSB=0):
  积不变: 0000 1011
  被乘数左移: 0010 1100
  乘数右移:       0011

时钟周期3 (乘数LSB=1):
  积 = 0000 1011 + 0010 1100 = 0011 0111
  被乘数左移: 0101 1000
  乘数右移:       0001

时钟周期4 (乘数LSB=1):
  积 = 0011 0111 + 0101 1000 = 1000 1111
  被乘数左移: 1011 0000
  乘数右移:       0000

最终结果: 1000 1111 (143) ✓
```

### 状态机图

```
                    ┌──────────┐
           Reset    │          │
         ┌─────────►│   IDLE   │
         │          │          │
         │          └────┬─────┘
         │               │ Start
         │               ▼
         │          ┌──────────┐
         │          │          │
         │          │  LOAD    │◄────────┐
         │          │ (加载操作数)│         │
         │          │          │         │
         │          └────┬─────┘         │
         │               │               │
         │               ▼               │
         │    ┌────┐  ┌──────────┐       │
         │    │    │  │          │       │
         │    │ No │◄─┤   TEST   │       │
         │    │    │  │ (检查LSB) │       │
         │    └──┬─┘  └────┬─────┘       │
         │       │         │ Yes         │
         │       │         ▼             │
         │       │    ┌──────────┐       │
         │       │    │          │       │
         │       │    │   ADD    │───────┤
         │       │    │ (执行加法) │       │
         │       │    │          │       │
         │       │    └────┬─────┘       │
         │       │         │             │
         │       │         ▼             │
         │       │    ┌──────────┐       │
         │       └───►│          │       │
         │            │  SHIFT   │───────┤
         │            │ (移位操作) │       │
         │            │          │       │
         │            └────┬─────┘       │
         │                 │             │
         │           Counter>0?          │
         │                 │ Yes         │
         └─────────────────┘             │
                           No            │
                           ▼             │
                      ┌──────────┐       │
                      │          │       │
                      │  DONE    ├───────┘
                      │ (完成)    │
                      └──────────┘
```

### Verilog实现

```verilog
// ============================================================
// 移位乘法器 (Shift-and-Add Multiplier)
// ============================================================

module shift_add_multiplier (
    input  wire        clk,
    input  wire        rst_n,
    input  wire        start,
    input  wire [7:0]  multiplicand,  // 被乘数
    input  wire [7:0]  multiplier,    // 乘数
    output reg  [15:0] product,       // 乘积 (2n位)
    output reg         done
);

    // 状态定义
    localparam IDLE = 2'b00;
    localparam CALC = 2'b01;
    localparam DONE = 2'b10;

    // 寄存器
    reg [1:0]  state;
    reg [15:0] mcand_reg;   // 被乘数寄存器 (2n位，左移)
    reg [7:0]  mplier_reg;  // 乘数寄存器 (n位，右移)
    reg [3:0]  counter;     // 计数器

    // 状态机
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state <= IDLE;
            done <= 1'b0;
            product <= 16'b0;
        end else begin
            case (state)
                IDLE: begin
                    done <= 1'b0;
                    if (start) begin
                        state <= CALC;
                        mcand_reg <= {8'b0, multiplicand};
                        mplier_reg <= multiplier;
                        product <= 16'b0;
                        counter <= 4'd8;  // 8位乘法
                    end
                end

                CALC: begin
                    // 如果乘数最低位为1，将被乘数加到积
                    if (mplier_reg[0]) begin
                        product <= product + mcand_reg;
                    end

                    // 被乘数左移，乘数右移
                    mcand_reg <= mcand_reg << 1;
                    mplier_reg <= mplier_reg >> 1;

                    // 计数器递减
                    counter <= counter - 1'b1;

                    // 检查是否完成
                    if (counter == 4'd1) begin
                        state <= DONE;
                    end
                end

                DONE: begin
                    done <= 1'b1;
                    state <= IDLE;
                end

                default: state <= IDLE;
            endcase
        end
    end

endmodule

// ============================================================
// 移位乘法器测试平台
// ============================================================

`timescale 1ns/1ps

module shift_add_multiplier_tb;

    // 测试信号
    reg         clk;
    reg         rst_n;
    reg         start;
    reg  [7:0]  multiplicand;
    reg  [7:0]  multiplier;
    wire [15:0] product;
    wire        done;

    // 实例化被测模块
    shift_add_multiplier uut (
        .clk(clk),
        .rst_n(rst_n),
        .start(start),
        .multiplicand(multiplicand),
        .multiplier(multiplier),
        .product(product),
        .done(done)
    );

    // 时钟生成
    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end

    // 测试过程
    initial begin
        $display("==============================================================");
        $display("              移位乘法器功能测试");
        $display("==============================================================");
        $display("  被乘数  │  乘数  │  预期结果  │ 实际结果  │ 状态");
        $display("--------------------------------------------------------------");

        rst_n = 0;
        start = 0;
        #20;
        rst_n = 1;
        #10;

        // 测试用例1: 简单乘法
        test_multiply(8'd10, 8'd20);

        // 测试用例2: 边界值
        test_multiply(8'd255, 8'd255);

        // 测试用例3: 乘以0
        test_multiply(8'd100, 8'd0);

        // 测试用例4: 乘以1
        test_multiply(8'd123, 8'd1);

        // 测试用例5: 随机测试
        test_multiply(8'hAB, 8'hCD);

        $display("==============================================================");
        $finish;
    end

    // 测试任务
    task test_multiply;
        input [7:0] mcand;
        input [7:0] mplier;
        reg [15:0] expected;
        begin
            multiplicand = mcand;
            multiplier = mplier;
            expected = mcand * mplier;

            @(posedge clk);
            start = 1;
            @(posedge clk);
            start = 0;

            // 等待完成
            wait(done);
            @(posedge clk);

            if (product === expected) begin
                $display("   %3d    │  %3d   │    %5d   │   %5d   │ PASS",
                         mcand, mplier, expected, product);
            end else begin
                $display("   %3d    │  %3d   │    %5d   │   %5d   │ FAIL",
                         mcand, mplier, expected, product);
            end

            #20;
        end
    endtask

    // 波形输出
    initial begin
        $dumpfile("shift_add_multiplier.vcd");
        $dumpvars(0, shift_add_multiplier_tb);
    end

endmodule
```

### 时序图

```
    移位乘法器时序图 (8位 × 8位):

    Clock:  ─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐
            └─┘ └─┘ └─┘ └─┘ └─┘ └─┘ └─┘ └─┘ └─┘ └─┘ └─┘ └─┘ └─┘ └─┘ └─┘

    Start:  ───┐     ┌───────────────────────────────────────────────────
               └─────┘

    State:  IDLE  LOAD   CALC    CALC    CALC    ...    CALC    DONE   IDLE

    Counter:  X    8      7       6       5              1       0      X

    mplier_reg:
            X   1101    0110    0011    0001    ...    0000

    LSB:        1       0       1       1       ...     0

    mcand_reg:
            X  00001011 00010110 00101100 01011000 ... 00000000

    Product:0   0      1011    1011    110111  10001111 10001111

    Done:   ───────────────────────────────────────────────────┐     ┌───
                                                               └─────┘

    总周期数 = 位数 + 2 (LOAD和DONE周期)
```

---

## 阵列乘法器

### 原理

阵列乘法器（Array Multiplier）通过组合逻辑阵列实现乘法，所有部分积同时计算并累加，实现纯组合逻辑的乘法运算。

### 结构图

```
    4×4阵列乘法器结构:

    A3 ─────────────────────────►
    A2 ───────────────────────►
    A1 ─────────────────────►
    A0 ───────────────────►
              │   │   │   │
    B0 ───────▼───▼───▼───▼───► AND阵列
              │   │   │   │
    B1 ───────▼───▼───▼───▼───►
              │   │   │   │
    B2 ───────▼───▼───▼───▼───►
              │   │   │   │
    B3 ───────▼───▼───▼───▼───►
              │   │   │   │
              ▼   ▼   ▼   ▼
            ┌───┬───┬───┬───┐
            │AND│AND│AND│AND│
            └───┴───┴───┴───┘
              │   │   │   │
              ▼   ▼   ▼   ▼
            ┌───┬───┬───┬───┐
            │FA │FA │FA │HA │  ◄── 第0行部分积
            └───┴───┴───┴───┘
              │   │   │   │
              ▼   ▼   ▼   ▼
            ┌───┬───┬───┬───┐
            │FA │FA │FA │FA │  ◄── 第1行部分积
            └───┴───┴───┴───┘
              │   │   │   │
              ▼   ▼   ▼   ▼
            ┌───┬───┬───┬───┐
            │FA │FA │FA │FA │  ◄── 第2行部分积
            └───┴───┴───┴───┘
              │   │   │   │
              ▼   ▼   ▼   ▼
    P7 P6   P5  P4  P3  P2  P1  P0
```

### 详细电路图

```
    4×4阵列乘法器详细电路:

         A3    A2    A1    A0
         │     │     │     │
         ▼     ▼     ▼     ▼
    B0 ─►○───►○───►○───►○    ┌─────┐
         │     │     │     │   │ a3b0│
         │     │     │     └──►│ a2b0│────┐
         │     │     └────────►│ a1b0│───┐│
         │     └──────────────►│ a0b0│──┐││
         │                       └─────┘  │││
         │                          │     │││
    B1 ─►○───►○───►○───►○           ▼     │││
         │     │     │     │      ┌────┐   │││
         │     │     │     └─────►│ HA │   │││ P0
         │     │     └───────────►│    ├───┘││
         │     └─────────────────►│ a  │    ││
         │      ┌────────────────►│ b  │    ││
         │      │                 └─┬──┘    ││
         │      │                   │c      ││
         │      ▼                   ▼       ││
         │    ┌────┐              ┌────┐    ││
         │    │AND │─────────────►│ FA │    ││
         │    │a3b1│              │    ├────┘│
         │    └────┘─────────────►│ a  │     │
         │    ┌────┐─────────────►│ b  │     │
         │    │a2b1│              └─┬──┘     │
         │    └────┘                │c       │
         │    ┌────┐                ▼        │
         └────┤a1b1│              ┌────┐     │
              └────┘─────────────►│ FA │     │
         ┌────┤a0b1│              │    ├─────┘ P1
    ...  │    └────┘─────────────►│ a  │
         │                        │ b  │
         │                        └─┬──┘
         │                          │c
         │                          ▼
         │                        ...
         │
    B3 ─►○───►○───►○───►○
         │     │     │     │
         │     │     │     └────► ...
         │     │     └──────────►
         │     └────────────────►
         └──────────────────────►

    P7    P6    P5    P4    P3    P2
```

### 斜向进位结构

```
    斜向进位阵列乘法器 (更优的延迟):

               A3B0  A2B0  A1B0  A0B0
                  ╲   ╱     ╲   ╱
                   ╲ ╱       ╲ ╱
                   ┌───┐     ┌───┐
                   │HA │     │HA │
                   └─┬─┘     └─┬─┘
                     │         │
    A3B1  A2B1  A1B1  A0B1     │
       ╲   ╱     ╲   ╱        │
        ╲ ╱       ╲ ╱         │
        ┌───┐     ┌───┐       │
        │FA │────►│FA │       │
        └─┬─┘     └─┬─┘       │
          │         │         │
    A3B2  A2B2  A1B2  A0B2    │
       ╲   ╱     ╲   ╱        │
        ╲ ╱       ╲ ╱         │
        ┌───┐     ┌───┐       │
        │FA │────►│FA │       │
        └─┬─┘     └─┬─┘       │
          │         │         │
    A3B3  A2B3  A1B3  A0B3    │
       ╲   ╱     ╲   ╱        │
        ╲ ╱       ╲ ╱         │
        ┌───┐     ┌───┐       │
        │FA │────►│FA │◄──────┘
        └─┬─┘     └─┬─┘
          │         │
          ▼         ▼
          P         P

    延迟 = (n-1) × t_FA + t_HA (斜向进位)
```

### Verilog实现

```verilog
// ============================================================
// 4×4阵列乘法器 (Array Multiplier)
// ============================================================

module array_multiplier_4x4 (
    input  wire [3:0] a,      // 被乘数
    input  wire [3:0] b,      // 乘数
    output wire [7:0] product // 乘积
);

    // 部分积
    wire [3:0] pp0, pp1, pp2, pp3;

    // 生成部分积 (AND阵列)
    assign pp0 = {a[3]&b[0], a[2]&b[0], a[1]&b[0], a[0]&b[0]};
    assign pp1 = {a[3]&b[1], a[2]&b[1], a[1]&b[1], a[0]&b[1]};
    assign pp2 = {a[3]&b[2], a[2]&b[2], a[1]&b[2], a[0]&b[2]};
    assign pp3 = {a[3]&b[3], a[2]&b[3], a[1]&b[3], a[0]&b[3]};

    // 内部连线
    wire [3:0] s1, s2, s3;
    wire [3:0] c1, c2, c3;

    // 第0行: 半加器 (最低有效位直接输出)
    half_adder ha0 (.a(pp0[1]), .b(pp1[0]), .sum(s1[0]), .carry(c1[0]));
    full_adder fa0 (.a(pp0[2]), .b(pp1[1]), .cin(pp2[0]), .sum(s1[1]), .cout(c1[1]));
    full_adder fa1 (.a(pp0[3]), .b(pp1[2]), .cin(pp2[1]), .sum(s1[2]), .cout(c1[2]));
    full_adder fa2 (.a(1'b0),   .b(pp1[3]), .cin(pp2[2]), .sum(s1[3]), .cout(c1[3]));

    // 第1行
    half_adder ha1 (.a(s1[1]), .b(c1[0]), .sum(s2[0]), .carry(c2[0]));
    full_adder fa3 (.a(s1[2]), .b(c1[1]), .cin(pp3[0]), .sum(s2[1]), .cout(c2[1]));
    full_adder fa4 (.a(s1[3]), .b(c1[2]), .cin(pp3[1]), .sum(s2[2]), .cout(c2[2]));
    full_adder fa5 (.a(c1[3]), .b(pp2[3]), .cin(pp3[2]), .sum(s2[3]), .cout(c2[3]));

    // 第2行
    half_adder ha2 (.a(s2[1]), .b(c2[0]), .sum(s3[0]), .carry(c3[0]));
    full_adder fa6 (.a(s2[2]), .b(c2[1]), .cin(c3[0]), .sum(s3[1]), .cout(c3[1]));
    full_adder fa7 (.a(s2[3]), .b(c2[2]), .cin(c3[1]), .sum(s3[2]), .cout(c3[2]));
    full_adder fa8 (.a(c2[3]), .b(pp3[3]), .cin(c3[2]), .sum(s3[3]), .cout(c3[3]));

    // 组合输出
    assign product[0] = pp0[0];
    assign product[1] = s1[0];
    assign product[2] = s2[0];
    assign product[3] = s3[0];
    assign product[4] = s3[1];
    assign product[5] = s3[2];
    assign product[6] = s3[3];
    assign product[7] = c3[3];

endmodule

// ============================================================
// 8×8 Wallace树乘法器 (高速版本)
// ============================================================

module wallace_multiplier_8x8 (
    input  wire [7:0]  a,
    input  wire [7:0]  b,
    output wire [15:0] product
);

    // 生成64个部分积位
    wire [63:0] pp;
    generate
        genvar i, j;
        for (i = 0; i < 8; i = i + 1) begin : row
            for (j = 0; j < 8; j = j + 1) begin : col
                assign pp[i*8+j] = a[j] & b[i];
            end
        end
    endgenerate

    // 第一级：使用全加器压缩部分积
    // (简化表示，实际Wallace树需要多层压缩)

    wire [15:0] sum_level1, carry_level1;

    // 使用generate生成Wallace树结构
    // 这里使用简化的行为级描述

    assign {carry_level1, sum_level1} = a * b;

    // 最终加法
    assign product = sum_level1 + (carry_level1 << 1);

endmodule

// ============================================================
// 阵列乘法器测试平台
// ============================================================

`timescale 1ns/1ps

module array_multiplier_tb;

    // 测试信号
    reg  [3:0] a;
    reg  [3:0] b;
    wire [7:0] product;

    // 实例化被测模块
    array_multiplier_4x4 uut (
        .a(a),
        .b(b),
        .product(product)
    );

    // 测试过程
    initial begin
        $display("==============================================================");
        $display("              4×4阵列乘法器功能测试");
        $display("==============================================================");
        $display("  A(hex) │ B(hex) │ Product(hex) │ 十进制验证");
        $display("--------------------------------------------------------------");

        // 遍历所有可能的输入组合
        for (a = 0; a < 16; a = a + 1) begin
            for (b = 0; b < 16; b = b + 1) begin
                #1;
                if (product !== a * b) begin
                    $display("ERROR: %h × %h = %h, expected %h", a, b, product, a*b);
                end
            end
        end

        // 显示部分测试用例
        a = 4'd10; b = 4'd12; #1;
        $display("   %h    │   %h    │     %h      │ %0d × %0d = %0d", a, b, product, a, b, product);

        a = 4'd15; b = 4'd15; #1;
        $display("   %h    │   %h    │     %h      │ %0d × %0d = %0d", a, b, product, a, b, product);

        a = 4'd7; b = 4'd8; #1;
        $display("   %h    │   %h    │     %h      │ %0d × %0d = %0d", a, b, product, a, b, product);

        a = 4'd0; b = 4'd9; #1;
        $display("   %h    │   %h    │     %h      │ %0d × %0d = %0d", a, b, product, a, b, product);

        $display("==============================================================");
        $display("所有256种组合测试完成！");
        $finish;
    end

    // 波形输出
    initial begin
        $dumpfile("array_multiplier.vcd");
        $dumpvars(0, array_multiplier_tb);
    end

endmodule
```

### 时序图

```
    阵列乘法器时序 (纯组合逻辑):

    A/B输入:  ────┐                          ┌───────────
                └──────────────────────────┘

    部分积:   ─────────┐  ┌──┐  ┌───────────┐
                     └──┘  └──┘

    进位链:   ─────────────┐  ┌──┐  ┌───────┐
                         └──┘  └──┘

    Product:  ─────────────────┐  ┌──┐  ┌───┐
  (稳定)                       └──┘  └──┘

    总延迟 = (n-1) × t_FA + t_AND
           ≈ 7 × 150ps + 30ps = 1.08ns (对于8×8)

    注意：由于是组合逻辑，没有时钟边沿触发
    输出在经过传播延迟后稳定
```

---

## Booth算法

### 原理

Booth算法通过检查乘数的位对来减少部分积的数量，特别适用于有符号数乘法，并可以处理连续的1或0序列来加速运算。

### 编码规则

```
Booth编码表 (当前位 vs 前一位):

┌──────────────┬──────────────┬─────────────────────────────────────┐
│  当前位 Y[i] │ 前一位 Y[i-1] │          操作                      │
├──────────────┼──────────────┼─────────────────────────────────────┤
│      0       │      0       │  0 × 被乘数 (无操作)               │
│      0       │      1       │  +1 × 被乘数 (加被乘数)            │
│      1       │      0       │  -1 × 被乘数 (减被乘数)            │
│      1       │      1       │  0 × 被乘数 (无操作)               │
└──────────────┴──────────────┴─────────────────────────────────────┘

观察：
- 01 表示一串1的结束，执行加法
- 10 表示一串1的开始，执行减法
- 00 和 11 在1串的中间，无操作
```

### 示例

```
6位乘法: 011011 (27) × 101100 (-20)

乘数: 1 0 1 1 0 0 0 (补一位0)
      ↑ ↑ ↑ ↑ ↑ ↑
      0 1 0 1 0 0  (位对)

解释:
i=0: 00 → 无操作
i=1: 00 → 无操作
i=2: 10 → 减被乘数 (Y+=-M)
i=3: 01 → 加被乘数 (Y+=+M)
i=4: 10 → 减被乘数 (Y+=-2M)
i=5: 01 → 加被乘数 (Y+=+4M)
```

### 改进Booth算法 (基数-4)

```
每次检查3位，产生5种操作:

┌─────────────────────────────────────────────┐
│  位组 (Y[i+1],Y[i],Y[i-1])  │  操作         │
├─────────────────────────────┼───────────────┤
│           000               │  0            │
│           001               │  +M           │
│           010               │  +M           │
│           011               │  +2M          │
│           100               │  -2M          │
│           101               │  -M           │
│           110               │  -M           │
│           111               │  0            │
└─────────────────────────────┴───────────────┘

优势：部分积数量减半，速度加倍
```

### 数据通路图

```
    ┌─────────────────────────────────────────────────────────────────────┐
    │                     Booth乘法器数据通路                              │
    │                                                                     │
    │   ┌─────────────┐         ┌─────────────┐                          │
    │   │   被乘数寄存器 │         │   乘数寄存器   │                          │
    │   │    (M)      │         │    (Q)       │                          │
    │   │             │         │  ┌───────┐   │                          │
    │   │  011011     │         │  │1101000│   │  (补0后的乘数)            │
    │   │             │         │  └───────┘   │                          │
    │   └──────┬──────┘         └──────┬──────┘                          │
    │          │                       │                                 │
    │          │                       ▼                                 │
    │          │                ┌─────────────┐                          │
    │          │                │  Booth编码器 │                          │
    │          │                │             │                          │
    │          │                │ 检查Q[2:0]  │                          │
    │          │                └──────┬──────┘                          │
    │          │                       │                                 │
    │          │              ┌────────┴────────┐                        │
    │          │              ▼                 ▼                        │
    │          │        ┌─────────┐      ┌─────────┐                     │
    │          │        │  +M     │      │  -M     │                     │
    │          │        │  (0)    │      │  (1)    │                     │
    │          │        └────┬────┘      └────┬────┘                     │
    │          │             │                │                          │
    │          │             └────────┬───────┘                          │
    │          │                      │                                  │
    │          │                      ▼                                  │
    │          │              ┌─────────────┐                            │
    │          └─────────────►│   加法器     │                            │
    │                         │  (加/减)    │                            │
    │    ┌────────────────────┤             │                            │
    │    │                    └──────┬──────┘                            │
    │    │                           │                                   │
    │    │    ┌──────────────────────┘                                   │
    │    │    │                                                         │
    │    │    ▼                                                         │
    │    │  ┌─────────────┐                                             │
    │    │  │    积寄存器  │                                             │
    │    │  │   (2n位)    │                                             │
    │    │  │             │                                             │
    │    └──┤  右移2位    │                                             │
    │       └─────────────┘                                             │
    │                                                                     │
    └─────────────────────────────────────────────────────────────────────┘
```

### Verilog实现

```verilog
// ============================================================
// 改进Booth算法乘法器 ( radix-4 )
// ============================================================

module booth_multiplier (
    input  wire        clk,
    input  wire        rst_n,
    input  wire        start,
    input  wire [15:0] multiplicand,
    input  wire [15:0] multiplier,
    output reg  [31:0] product,
    output reg         done
);

    // 状态定义
    localparam IDLE = 3'b000;
    localparam INIT = 3'b001;
    localparam CALC = 3'b010;
    localparam SHIFT = 3'b011;
    localparam DONE = 3'b100;

    // 寄存器
    reg [2:0]  state;
    reg [16:0] multiplier_reg;  // 乘数 + 额外位
    reg [15:0] multiplicand_reg;
    reg [31:0] accum;           // 累加器
    reg [3:0]  count;           // 计数器 (16位/2 = 8次迭代)

    // Booth编码逻辑
    wire [2:0] booth_bits = multiplier_reg[2:0];
    reg  [16:0] add_val;        // 要加的值 (带符号扩展)

    always @(*) begin
        case (booth_bits)
            3'b000: add_val = 17'b0;                    // 0
            3'b001: add_val = {multiplicand_reg[15], multiplicand_reg}; // +M
            3'b010: add_val = {multiplicand_reg[15], multiplicand_reg}; // +M
            3'b011: add_val = {multiplicand_reg, 1'b0}; // +2M
            3'b100: add_val = -{multiplicand_reg, 1'b0}; // -2M
            3'b101: add_val = -{multiplicand_reg[15], multiplicand_reg}; // -M
            3'b110: add_val = -{multiplicand_reg[15], multiplicand_reg}; // -M
            3'b111: add_val = 17'b0;                    // 0
            default: add_val = 17'b0;
        endcase
    end

    // 状态机
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state <= IDLE;
            done <= 1'b0;
            product <= 32'b0;
            accum <= 32'b0;
        end else begin
            case (state)
                IDLE: begin
                    done <= 1'b0;
                    if (start) begin
                        state <= INIT;
                    end
                end

                INIT: begin
                    multiplicand_reg <= multiplicand;
                    multiplier_reg <= {multiplier, 1'b0};  // 补0
                    accum <= 32'b0;
                    count <= 4'd8;  // 16位需要8次迭代
                    state <= CALC;
                end

                CALC: begin
                    // 加/减操作
                    accum[31:15] <= accum[31:15] + add_val;
                    state <= SHIFT;
                end

                SHIFT: begin
                    // 算术右移2位
                    accum <= {{2{accum[31]}}, accum[31:2]};
                    multiplier_reg <= {2'b00, multiplier_reg[16:2]};

                    count <= count - 1'b1;
                    if (count == 4'd1) begin
                        state <= DONE;
                    end else begin
                        state <= CALC;
                    end
                end

                DONE: begin
                    product <= accum;
                    done <= 1'b1;
                    state <= IDLE;
                end

                default: state <= IDLE;
            endcase
        end
    end

endmodule

// ============================================================
// Booth乘法器测试平台
// ============================================================

`timescale 1ns/1ps

module booth_multiplier_tb;

    // 测试信号
    reg         clk;
    reg         rst_n;
    reg         start;
    reg  [15:0] multiplicand;
    reg  [15:0] multiplier;
    wire [31:0] product;
    wire        done;

    // 实例化被测模块
    booth_multiplier uut (
        .clk(clk),
        .rst_n(rst_n),
        .start(start),
        .multiplicand(multiplicand),
        .multiplier(multiplier),
        .product(product),
        .done(done)
    );

    // 时钟生成
    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end

    // 测试过程
    initial begin
        $display("==============================================================");
        $display("            改进Booth算法乘法器功能测试");
        $display("==============================================================");
        $display("  被乘数   │   乘数   │   预期结果   │   实际结果   │ 状态");
        $display("--------------------------------------------------------------");

        rst_n = 0;
        start = 0;
        #20;
        rst_n = 1;
        #10;

        // 正数乘法
        test_multiply(16'd100, 16'd200);

        // 有符号数乘法
        test_signed(-16'd50, 16'd30);
        test_signed(16'd25, -16'd40);
        test_signed(-16'd100, -16'd50);

        // 边界值
        test_multiply(16'h7FFF, 16'd2);   // 最大正数 × 2
        test_multiply(16'h8000, 16'd1);   // 最小负数

        // 大数乘法
        test_multiply(16'd12345, 16'd6789);

        $display("==============================================================");
        $finish;
    end

    // 有符号测试任务
    task test_signed;
        input signed [15:0] mcand;
        input signed [15:0] mplier;
        reg signed [31:0] expected;
        begin
            multiplicand = mcand;
            multiplier = mplier;
            expected = mcand * mplier;

            @(posedge clk);
            start = 1;
            @(posedge clk);
            start = 0;

            wait(done);
            @(posedge clk);

            if ($signed(product) === expected) begin
                $display(" %6d   │  %6d  │  %11d │  %11d  │ PASS",
                         mcand, mplier, expected, $signed(product));
            end else begin
                $display(" %6d   │  %6d  │  %11d │  %11d  │ FAIL",
                         mcand, mplier, expected, $signed(product));
            end

            #20;
        end
    endtask

    // 无符号测试任务
    task test_multiply;
        input [15:0] mcand;
        input [15:0] mplier;
        reg [31:0] expected;
        begin
            multiplicand = mcand;
            multiplier = mplier;
            expected = mcand * mplier;

            @(posedge clk);
            start = 1;
            @(posedge clk);
            start = 0;

            wait(done);
            @(posedge clk);

            if (product === expected) begin
                $display(" %6d   │  %6d  │  %11d │  %11d  │ PASS",
                         mcand, mplier, expected, product);
            end else begin
                $display(" %6d   │  %6d  │  %11d │  %11d  │ FAIL",
                         mcand, mplier, expected, product);
            end

            #20;
        end
    endtask

    // 波形输出
    initial begin
        $dumpfile("booth_multiplier.vcd");
        $dumpvars(0, booth_multiplier_tb);
    end

endmodule
```

---

## 恢复余数除法器

### 原理

恢复余数除法器（Restoring Division）是最基本的除法算法，通过迭代地减去除数并恢复负的余数来完成除法运算。

### 算法流程

```
初始化:
  被除数 (Dividend) = N
  除数 (Divisor) = D
  余数 (Remainder) = 0
  商 (Quotient) = 0
  计数器 = n (位数)

算法:
  1. 将被除数左移1位，最高位移入余数
  2. 余数 = 余数 - 除数
  3. 如果余数 < 0:
       余数 = 余数 + 除数 (恢复)
       商最低位 = 0
     否则:
       商最低位 = 1
  4. 计数器--，如果 > 0 回到步骤1
```

### 数据通路图

```
    ┌─────────────────────────────────────────────────────────────────────┐
    │                    恢复余数除法器数据通路                            │
    │                                                                     │
    │   ┌─────────────┐         ┌─────────────┐                          │
    │   │   余数寄存器   │         │   商寄存器    │                          │
    │   │   (n+1位)    │         │   (n位)      │                          │
    │   │             │         │             │                          │
    │   │  ┌───────┐  │         │  ┌───────┐  │                          │
    │   │  │0000000│  │         │  │0000000│  │                          │
    │   │  └───────┘  │         │  └───────┘  │                          │
    │   │      │      │         │      ▲      │                          │
    │   │      │      │         │      │LSB   │                          │
    │   │      ▼      │         │      │      │                          │
    │   │  ┌───────┐  │         │  ┌───────┐  │                          │
    │   │  │ 左移  │  │         │  │ 左移  │  │                          │
    │   │  │  1位  │  │         │  │  1位  │  │                          │
    │   │  └───┬───┘  │         │  └───┬───┘  │                          │
    │   │      │      │         │      │      │                          │
    │   └──────┼──────┘         └──────┼──────┘                          │
    │          │                        │                                 │
    │          │     ┌──────────────────┘                                 │
    │          │     │                                                   │
    │          │     │  ┌─────────┐                                      │
    │          │     └──┤  减法器  │◄───────────────                      │
    │          │        │  R - D  │               │                      │
    │          │        └───┬─────┘               │                      │
    │          │            │                     │                      │
    │          │       ┌────┴────┐                │                      │
    │          │       │         │                │                      │
    │          │    ┌──┴──┐   ┌──┴──┐            │                      │
    │          │    │ <0  │   │ >=0 │            │                      │
    │          │    └──┬──┘   └──┬──┘            │                      │
    │          │       │         │               │                      │
    │          │    ┌──┴──┐      │               │                      │
    │          │    │恢复 │      │               │                      │
    │          │    │R=R+D│      │               │                      │
    │          │    └──┬──┘      │               │                      │
    │          └───────┼─────────┘               │                      │
    │                  │                         │                      │
    │                  └──────────┬──────────────┘                      │
    │                             │                                     │
    │   ┌─────────────┐           │                                     │
    │   │   除数寄存器  │◄──────────┘                                     │
    │   │    (-D)     │                                                 │
    │   │             │                                                 │
    │   └─────────────┘                                                 │
    │                                                                     │
    └─────────────────────────────────────────────────────────────────────┘
```

### 详细运算过程示例

```
7 ÷ 3 (0111 ÷ 0011)

初始化:
  余数 = 0000
  商 = 0000
  除数 = 0011

迭代过程:

周期1:
  余数:商左移: 0000:0111 → 0000:1110 (被除数最高位0移入)
  余数 = 0000 - 0011 = 1101 (负数!)
  恢复: 余数 = 1101 + 0011 = 0000
  商LSB = 0
  → 余数=0000, 商=0010

周期2:
  余数:商左移: 0000:0010 → 0000:0100 (被除数下一位0移入)
  余数 = 0000 - 0011 = 1101 (负数!)
  恢复: 余数 = 1101 + 0011 = 0000
  商LSB = 0
  → 余数=0000, 商=0100

周期3:
  余数:商左移: 0000:0100 → 0000:1001 (被除数下一位1移入)
  余数 = 0000 - 0011 = 1101 (负数!)
  恢复: 余数 = 1101 + 0011 = 0000
  商LSB = 0
  → 余数=0000, 商=1000

周期4:
  余数:商左移: 0000:1000 → 0001:0000 (被除数最后一位1移入)
  余数 = 0001 - 0011 = 1110 (负数!)
  恢复: 余数 = 1110 + 0011 = 0001
  商LSB = 0
  → 余数=0001, 商=0000

最终结果:
  商 = 0010 (2)
  余数 = 0001 (1)

验证: 7 = 3 × 2 + 1 ✓
```

### Verilog实现

```verilog
// ============================================================
// 恢复余数除法器 (Restoring Division)
// ============================================================

module restoring_divider (
    input  wire        clk,
    input  wire        rst_n,
    input  wire        start,
    input  wire [7:0]  dividend,    // 被除数
    input  wire [7:0]  divisor,     // 除数
    output reg  [7:0]  quotient,    // 商
    output reg  [7:0]  remainder,   // 余数
    output reg         done,
    output reg         divide_by_zero
);

    // 状态定义
    localparam IDLE = 3'b000;
    localparam CHECK = 3'b001;
    localparam SHIFT = 3'b010;
    localparam SUB = 3'b011;
    localparam RESTORE = 3'b100;
    localparam DONE = 3'b101;

    // 寄存器
    reg [2:0]  state;
    reg [7:0]  rem_reg;         // 余数寄存器
    reg [7:0]  quot_reg;        // 商寄存器
    reg [7:0]  divid_reg;       // 被除数寄存器
    reg [7:0]  divsr_reg;       // 除数寄存器
    reg [3:0]  count;           // 计数器
    reg [8:0]  temp;            // 临时结果 (9位用于检测符号)

    // 状态机
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state <= IDLE;
            done <= 1'b0;
            divide_by_zero <= 1'b0;
            quotient <= 8'b0;
            remainder <= 8'b0;
        end else begin
            case (state)
                IDLE: begin
                    done <= 1'b0;
                    divide_by_zero <= 1'b0;
                    if (start) begin
                        state <= CHECK;
                        divid_reg <= dividend;
                        divsr_reg <= divisor;
                        rem_reg <= 8'b0;
                        quot_reg <= 8'b0;
                        count <= 4'd8;
                    end
                end

                CHECK: begin
                    // 检查除数是否为0
                    if (divsr_reg == 8'b0) begin
                        divide_by_zero <= 1'b1;
                        state <= DONE;
                    end else begin
                        state <= SHIFT;
                    end
                end

                SHIFT: begin
                    // 余数:商左移，被除数最高位移入余数
                    rem_reg <= {rem_reg[6:0], divid_reg[7]};
                    quot_reg <= {quot_reg[6:0], 1'b0};
                    divid_reg <= {divid_reg[6:0], 1'b0};
                    state <= SUB;
                end

                SUB: begin
                    // 余数 = 余数 - 除数
                    temp <= {1'b0, rem_reg} - {1'b0, divsr_reg};
                    state <= RESTORE;
                end

                RESTORE: begin
                    if (temp[8]) begin  // 结果为负
                        // 恢复余数
                        rem_reg <= rem_reg;  // 保持不变
                        quot_reg[0] <= 1'b0;
                    end else begin
                        // 接受结果
                        rem_reg <= temp[7:0];
                        quot_reg[0] <= 1'b1;
                    end

                    count <= count - 1'b1;
                    if (count == 4'd1) begin
                        state <= DONE;
                    end else begin
                        state <= SHIFT;
                    end
                end

                DONE: begin
                    quotient <= quot_reg;
                    remainder <= rem_reg;
                    done <= 1'b1;
                    state <= IDLE;
                end

                default: state <= IDLE;
            endcase
        end
    end

endmodule

// ============================================================
// 恢复余数除法器测试平台
// ============================================================

`timescale 1ns/1ps

module restoring_divider_tb;

    // 测试信号
    reg         clk;
    reg         rst_n;
    reg         start;
    reg  [7:0]  dividend;
    reg  [7:0]  divisor;
    wire [7:0]  quotient;
    wire [7:0]  remainder;
    wire        done;
    wire        divide_by_zero;

    // 实例化被测模块
    restoring_divider uut (
        .clk(clk),
        .rst_n(rst_n),
        .start(start),
        .dividend(dividend),
        .divisor(divisor),
        .quotient(quotient),
        .remainder(remainder),
        .done(done),
        .divide_by_zero(divide_by_zero)
    );

    // 时钟生成
    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end

    // 测试过程
    initial begin
        $display("==============================================================");
        $display("            恢复余数除法器功能测试");
        $display("==============================================================");
        $display(" 被除数 │ 除数 │  商  │ 余数 │ 验证");
        $display("--------------------------------------------------------------");

        rst_n = 0;
        start = 0;
        #20;
        rst_n = 1;
        #10;

        // 基本测试
        test_divide(8'd100, 8'd3);
        test_divide(8'd255, 8'd16);
        test_divide(8'd50, 8'd7);
        test_divide(8'd128, 8'd2);

        // 边界测试
        test_divide(8'd0, 8'd5);
        test_divide(8'd5, 8'd5);

        // 除零测试
        test_divide_zero(8'd100, 8'd0);

        $display("==============================================================");
        $finish;
    end

    // 除法测试任务
    task test_divide;
        input [7:0] divid;
        input [7:0] divsr;
        reg [7:0] expected_q;
        reg [7:0] expected_r;
        begin
            dividend = divid;
            divisor = divsr;
            expected_q = divid / divsr;
            expected_r = divid % divsr;

            @(posedge clk);
            start = 1;
            @(posedge clk);
            start = 0;

            wait(done);
            @(posedge clk);

            if (quotient === expected_q && remainder === expected_r) begin
                $display("  %3d   │  %3d  │  %3d  │  %3d  │ %0d = %0d×%0d + %0d",
                         divid, divsr, quotient, remainder,
                         divid, divisor, quotient, remainder);
            end else begin
                $display("  %3d   │  %3d  │  %3d  │  %3d  │ FAIL (expected %0d, %0d)",
                         divid, divsr, quotient, remainder, expected_q, expected_r);
            end

            #20;
        end
    endtask

    // 除零测试任务
    task test_divide_zero;
        input [7:0] divid;
        input [7:0] divsr;
        begin
            dividend = divid;
            divisor = divsr;

            @(posedge clk);
            start = 1;
            @(posedge clk);
            start = 0;

            wait(done);
            @(posedge clk);

            if (divide_by_zero) begin
                $display("  %3d   │  %3d  │  ---  │  ---  │ 除零错误检测成功", divid, divsr);
            end else begin
                $display("  %3d   │  %3d  │  %3d  │  %3d  │ 除零错误检测失败", divid, divsr, quotient, remainder);
            end

            #20;
        end
    endtask

    // 波形输出
    initial begin
        $dumpfile("restoring_divider.vcd");
        $dumpvars(0, restoring_divider_tb);
    end

endmodule
```

---

## 非恢复余数除法器

### 原理

非恢复余数除法器（Non-Restoring Division）是对恢复余数算法的改进。当余数为负时，不立即恢复，而是在下一步执行加法操作。这样可以省去恢复步骤，提高速度。

### 算法流程

```
非恢复除法算法:

如果上一步余数为正:
  余数 = 余数 - 除数
  如果余数 >= 0: 商位 = 1
  否则: 商位 = 0

如果上一步余数为负:
  余数 = 余数 + 除数  (不恢复，直接加)
  如果余数 >= 0: 商位 = 1
  否则: 商位 = 0

最后一步如果余数为负，需要恢复
```

### 对比优势

```
恢复余数 vs 非恢复余数:

恢复余数:
  每个周期: 移位 → 减法 → 条件加法(恢复)
  平均周期操作数: 2.5 (50%概率需要恢复)

非恢复余数:
  每个周期: 移位 → 加法或减法
  平均周期操作数: 2
  最后可能需要一次恢复

速度提升: 约20%
```

### 数据通路图

```
    ┌─────────────────────────────────────────────────────────────────────┐
    │                    非恢复余数除法器数据通路                          │
    │                                                                     │
    │   ┌─────────────┐         ┌─────────────┐                          │
    │   │   余数寄存器   │         │   商寄存器    │                          │
    │   │   (带符号)   │         │             │                          │
    │   │             │         │             │                          │
    │   │  ┌───────┐  │         │  ┌───────┐  │                          │
    │   │  │0000000│  │         │  │0000000│  │                          │
    │   │  └───────┘  │         │  └───────┘  │                          │
    │   │      │      │         │      ▲      │                          │
    │   │      │      │         │      │LSB   │                          │
    │   │      ▼      │         │      │      │                          │
    │   │  ┌───────┐  │         │  ┌───────┐  │                          │
    │   │  │ 左移  │  │         │  │ 左移  │  │                          │
    │   │  │  1位  │  │         │  │  1位  │  │                          │
    │   │  └───┬───┘  │         │  └───┬───┘  │                          │
    │   │      │      │         │      │      │                          │
    │   └──────┼──────┘         └──────┼──────┘                          │
    │          │                        │                                 │
    │          │                        │                                 │
    │          │     ┌──────────────────┘                                 │
    │          │     │                                                   │
    │          │     │  ┌─────────────────────────────┐                   │
    │          │     └──┤        加减法器              │                   │
    │          │        │                             │                   │
    │          ├───────►│  如果余数>=0: 余数 - 除数   │                   │
    │          │        │  如果余数<0:  余数 + 除数   │                   │
    │          │        │                             │                   │
    │          │        └─────────────────────────────┘                   │
    │          │                      │                                   │
    │          │                      ▼                                   │
    │          │               ┌────────────┐                            │
    │          │               │  符号检测   │                            │
    │          │               │  >=0? <0?  │                            │
    │          │               └──────┬─────┘                            │
    │          │                      │                                   │
    │   ┌─────────────┐              │                                   │
    │   │   除数寄存器  │              │                                   │
    │   │             │◄─────────────┘                                   │
    │   └─────────────┘                                                   │
    │                                                                     │
    └─────────────────────────────────────────────────────────────────────┘
```

### Verilog实现

```verilog
// ============================================================
// 非恢复余数除法器 (Non-Restoring Division)
// ============================================================

module non_restoring_divider (
    input  wire        clk,
    input  wire        rst_n,
    input  wire        start,
    input  wire [7:0]  dividend,
    input  wire [7:0]  divisor,
    output reg  [7:0]  quotient,
    output reg  [7:0]  remainder,
    output reg         done,
    output reg         divide_by_zero
);

    // 状态定义
    localparam IDLE = 3'b000;
    localparam CHECK = 3'b001;
    localparam SHIFT = 3'b010;
    localparam OPERATE = 3'b011;
    localparam FINAL_RESTORE = 3'b100;
    localparam DONE = 3'b101;

    // 寄存器
    reg [2:0]  state;
    reg [8:0]  rem_reg;         // 余数寄存器 (9位带符号)
    reg [7:0]  quot_reg;
    reg [7:0]  divid_reg;
    reg [7:0]  divsr_reg;
    reg [3:0]  count;
    reg        rem_sign;        // 余数符号 (0=正, 1=负)

    // 状态机
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state <= IDLE;
            done <= 1'b0;
            divide_by_zero <= 1'b0;
        end else begin
            case (state)
                IDLE: begin
                    done <= 1'b0;
                    divide_by_zero <= 1'b0;
                    if (start) begin
                        state <= CHECK;
                        divid_reg <= dividend;
                        divsr_reg <= divisor;
                        rem_reg <= 9'b0;
                        quot_reg <= 8'b0;
                        rem_sign <= 1'b0;
                        count <= 4'd8;
                    end
                end

                CHECK: begin
                    if (divsr_reg == 8'b0) begin
                        divide_by_zero <= 1'b1;
                        state <= DONE;
                    end else begin
                        state <= SHIFT;
                    end
                end

                SHIFT: begin
                    // 余数:商左移
                    rem_reg <= {rem_reg[7:0], divid_reg[7]};
                    quot_reg <= {quot_reg[6:0], 1'b0};
                    divid_reg <= {divid_reg[6:0], 1'b0};
                    state <= OPERATE;
                end

                OPERATE: begin
                    // 根据上一步余数符号决定加或减
                    if (rem_sign) begin
                        // 余数为负，执行加法
                        rem_reg <= rem_reg + {1'b0, divsr_reg};
                    end else begin
                        // 余数为正，执行减法
                        rem_reg <= rem_reg - {1'b0, divsr_reg};
                    end

                    // 确定当前余数符号和商位
                    // (需要在下一个周期才能得到正确符号)
                    state <= OPERATE;  // 等待运算完成

                    // 简化为组合逻辑判断
                    if (rem_reg[8]) begin  // 结果为负
                        quot_reg[0] <= 1'b0;
                        rem_sign <= 1'b1;
                    end else begin
                        quot_reg[0] <= 1'b1;
                        rem_sign <= 1'b0;
                    end

                    count <= count - 1'b1;
                    if (count == 4'd1) begin
                        state <= FINAL_RESTORE;
                    end else begin
                        state <= SHIFT;
                    end
                end

                FINAL_RESTORE: begin
                    // 如果最后余数为负，需要恢复
                    if (rem_sign) begin
                        rem_reg <= rem_reg + {1'b0, divsr_reg};
                    end
                    state <= DONE;
                end

                DONE: begin
                    quotient <= quot_reg;
                    remainder <= rem_reg[7:0];
                    done <= 1'b1;
                    state <= IDLE;
                end

                default: state <= IDLE;
            endcase
        end
    end

endmodule

// ============================================================
// 除法器性能对比测试平台
// ============================================================

`timescale 1ns/1ps

module divider_comparison_tb;

    // 测试信号
    reg         clk;
    reg         rst_n;
    reg         start;
    reg  [7:0]  dividend;
    reg  [7:0]  divisor;

    wire [7:0]  quotient_rest;
    wire [7:0]  remainder_rest;
    wire        done_rest;

    wire [7:0]  quotient_nonrest;
    wire [7:0]  remainder_nonrest;
    wire        done_nonrest;

    // 恢复余数除法器
    restoring_divider restoring (
        .clk(clk),
        .rst_n(rst_n),
        .start(start),
        .dividend(dividend),
        .divisor(divisor),
        .quotient(quotient_rest),
        .remainder(remainder_rest),
        .done(done_rest),
        .divide_by_zero()
    );

    // 非恢复余数除法器
    non_restoring_divider non_restoring (
        .clk(clk),
        .rst_n(rst_n),
        .start(start),
        .dividend(dividend),
        .divisor(divisor),
        .quotient(quotient_nonrest),
        .remainder(remainder_nonrest),
        .done(done_nonrest),
        .divide_by_zero()
    );

    // 时钟生成
    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end

    // 测试过程
    initial begin
        $display("==============================================================");
        $display("            除法器性能对比测试");
        $display("==============================================================");
        $display(" 算法类型        │  被除数 │ 除数 │  商  │ 余数 │ 周期数");
        $display("--------------------------------------------------------------");

        rst_n = 0;
        start = 0;
        #20;
        rst_n = 1;
        #10;

        // 对比测试
        compare_dividers(8'd100, 8'd3);
        compare_dividers(8'd200, 8'd15);
        compare_dividers(8'd255, 8'd16);

        $display("==============================================================");
        $finish;
    end

    // 对比测试任务
    task compare_dividers;
        input [7:0] divid;
        input [7:0] divsr;
        integer start_time_rest, end_time_rest;
        integer start_time_non, end_time_non;
        begin
            dividend = divid;
            divisor = divsr;

            // 测试恢复余数除法器
            start_time_rest = $time;
            @(posedge clk);
            start = 1;
            @(posedge clk);
            start = 0;
            wait(done_rest);
            end_time_rest = $time;
            @(posedge clk);

            $display(" 恢复余数除法    │  %3d   │  %3d  │  %3d  │  %3d  │  %2d",
                     divid, divsr, quotient_rest, remainder_rest,
                     (end_time_rest - start_time_rest) / 10);

            #20;

            // 测试非恢复余数除法器
            start_time_non = $time;
            @(posedge clk);
            start = 1;
            @(posedge clk);
            start = 0;
            wait(done_nonrest);
            end_time_non = $time;
            @(posedge clk);

            $display(" 非恢复余数除法  │  %3d   │  %3d  │  %3d  │  %3d  │  %2d",
                     divid, divsr, quotient_nonrest, remainder_nonrest,
                     (end_time_non - start_time_non) / 10);

            $display("--------------------------------------------------------------");
            #20;
        end
    endtask

    // 波形输出
    initial begin
        $dumpfile("divider_comparison.vcd");
        $dumpvars(0, divider_comparison_tb);
    end

endmodule
```

---

## 性能对比

### 乘法器性能对比

```
┌─────────────────────────────────────────────────────────────────────┐
│                        乘法器性能对比                                │
├──────────────────┬─────────────┬─────────────┬──────────────────────┤
│    乘法器类型     │ 延迟/周期    │   面积      │    功耗              │
├──────────────────┼─────────────┼─────────────┼──────────────────────┤
│ 移位乘法器        │ O(n) 周期   │ 最小        │ 最低 (时序电路)      │
│ (串行)           │ n+2 周期    │ ~n位寄存器  │                      │
├──────────────────┼─────────────┼─────────────┼──────────────────────┤
│ 阵列乘法器        │ O(n) 延迟   │ O(n²)      │ 中等 (组合逻辑)      │
│ (组合)           │ ~nt_FA      │ n²个加法器  │                      │
├──────────────────┼─────────────┼─────────────┼──────────────────────┤
│ Wallace树        │ O(log n)    │ O(n²)      │ 较高 (并行度高)      │
│                  │ ~log(n)t_FA │ 类似阵列    │                      │
├──────────────────┼─────────────┼─────────────┼──────────────────────┤
│ Booth算法        │ O(n/2) 周期 │ 中等        │ 低 (迭代次数减半)    │
│ (radix-4)        │ n/2+2 周期  │ 编码器+加法 │                      │
└──────────────────┴─────────────┴─────────────┴──────────────────────┘
```

### 除法器性能对比

```
┌─────────────────────────────────────────────────────────────────────┐
│                        除法器性能对比                                │
├──────────────────┬─────────────┬─────────────┬──────────────────────┤
│    除法器类型     │ 延迟/周期    │   复杂度    │    特点              │
├──────────────────┼─────────────┼─────────────┼──────────────────────┤
│ 恢复余数除法      │ n+2 周期    │ 低          │ 50%概率需要恢复      │
│                  │ 每周期2-3操作 │           │ 简单可靠             │
├──────────────────┼─────────────┼─────────────┼──────────────────────┤
│ 非恢复余数除法    │ n+2 周期    │ 低          │ 无需恢复步骤         │
│                  │ 每周期2操作   │           │ 速度快约20%          │
├──────────────────┼─────────────┼─────────────┼──────────────────────┤
│ SRT除法          │ n 周期      │ 高          │ 每次产生多位商       │
│ (radix-4/8)      │ 每周期1-2操作 │           │ 现代CPU使用          │
├──────────────────┼─────────────┼─────────────┼──────────────────────┤
│ 牛顿迭代除法      │ O(log n)    │ 高          │ 先求倒数再乘         │
│                  │ 乘法器依赖    │           │ 浮点除法常用         │
└──────────────────┴─────────────┴─────────────┴──────────────────────┘
```

### 延迟与时钟频率关系

```
    不同乘法器延迟对比 (32位):

    延迟(ns)
    100 ┤                                          ╭──── 移位乘法器
        │                                    ╭────╯
     80 ┤                              ╭────╯
        │                        ╭────╯
     60 ┤                  ╭────╯
        │            ╭────╯
     40 ┤      ╭────╯                            ╭──── Wallace树
        │╭────╯                            ╭────╯
     20 ┤╯                           ╭────╯
        │                      ╭────╯
      0 ┼────────────────────────────────────────────► 时钟频率(MHz)
        0    50   100  150  200  250  300  350  400

    关键频率点:
    - 移位乘法器: 任何频率都适用 (时序电路)
    - 阵列乘法器: 最高约100MHz
    - Wallace树: 可达300MHz+
```

### 面积效率

```
    32位乘法器面积估算 (等效门数):

    ┌─────────────────────────────────────────────────────────┐
    │                                                         │
    │   面积                                                  │
    │  (K门)                                                  │
    │    50 ┤                                ┌──── Wallace树  │
    │       │                           ┌────┤                │
    │    40 ┤                      ┌────┤    │                │
    │       │                 ┌────┤    ├────┤                │
    │    30 ┤            ┌────┤    ├────┘    │                │
    │       │       ┌────┤    ├────┤         │                │
    │    20 ┤  ┌────┤    ├────┤    │         │                │
    │       │  │    ├────┘    │    │         │                │
    │    10 ┤──┤────┘         │    │         │                │
    │       │  │              │    │         │                │
    │     0 ┼──┴──────────────┴────┴─────────┴────────────────│
    │         移位    阵列     改进    Wallace   Booth        │
    │                      阵列                            +树  │
    │                                                         │
    └─────────────────────────────────────────────────────────┘
```

---

## 与C语言的关系

### 整数乘除法的硬件映射

```c
// C语言源代码
int a = 100, b = 25;
int prod = a * b;    // 乘法
int quot = a / b;    // 除法
int rem = a % b;     // 取模

// ARM64汇编代码
// MUL X2, X0, X1      ; 乘法 (可能用Wallace树或Booth)
// SDIV X3, X0, X1     ; 有符号除法 (SRT除法器)
// UDIV X4, X0, X1     ; 无符号除法
```

### 编译器优化

```c
// 强度削弱：乘除法转换为移位
int a = x * 8;       // 优化为: x << 3
int b = y / 16;      // 优化为: y >> 4 (正数)
int c = z % 32;      // 优化为: z & 31

// 但以下不会优化
int d = x * 7;       // 可能优化为: (x << 3) - x
int e = y / 10;      // 无简单移位等价，使用除法器

// 使用内建函数指导优化
int f = __builtin_clz(x);  // 前导零计数，可能有专用指令
```

### 性能考虑

```c
// 避免除法在循环中
// 低效写法:
for (int i = 0; i < n; i++) {
    result[i] = array[i] / divisor;  // 每次迭代都除法
}

// 优化写法:
float inv_divisor = 1.0f / divisor;  // 一次除法
for (int i = 0; i < n; i++) {
    result[i] = array[i] * inv_divisor;  // 乘法代替
}

// 编译器自动向量化
// 现代编译器可以将简单循环转换为SIMD指令
// 一次处理多个乘除法
```

### 整数溢出行为

```c
#include <stdio.h>
#include <stdint.h>

int main() {
    // 乘法溢出
    int32_t a = 50000;
    int32_t b = 50000;
    int32_t prod = a * b;  // 溢出! 2,500,000,000 > INT_MAX
    printf("%d * %d = %d\n", a, b, prod);  // 未定义行为

    // 安全检测
    if (a > INT32_MAX / b) {
        printf("Multiplication would overflow!\n");
    }

    // 使用更大类型
    int64_t safe_prod = (int64_t)a * b;
    printf("Safe: %ld\n", safe_prod);

    // 除法溢出 (特殊情况)
    int32_t min = INT32_MIN;
    int32_t neg1 = -1;
    // int32_t bad = min / neg1;  // 溢出! |INT_MIN| > INT_MAX

    return 0;
}
```

### 硬件乘法器的使用

```
    ┌─────────────────────────────────────────────────────────────┐
    │                   CPU中的乘除法单元                          │
    ├─────────────────────────────────────────────────────────────┤
    │                                                             │
    │   ┌─────────────────┐      ┌─────────────────┐             │
    │   │   整数乘法器     │      │   整数除法器     │             │
    │   │                 │      │                 │             │
    │   │  ┌───────────┐  │      │  ┌───────────┐  │             │
    │   │  │ Booth或   │  │      │  │ SRT除法   │  │             │
    │   │  │ Wallace树 │  │      │  │ 迭代算法  │  │             │
    │   │  │ (3-5周期) │  │      │  │ (20-30周期)│  │             │
    │   │  └───────────┘  │      │  └───────────┘  │             │
    │   │                 │      │                 │             │
    │   │  32×32→64位    │      │  32÷32→32位    │             │
    │   └─────────────────┘      └─────────────────┘             │
    │                                                             │
    │   流水线设计:                                               │
    │   - 乘法器通常完全流水线化，每周期可开始新运算              │
    │   - 除法器通常非流水线化，占用多个周期                      │
    │                                                             │
    └─────────────────────────────────────────────────────────────┘
```

---

## 总结

本文档详细介绍了计算机中的乘除法实现，包括：

1. **移位乘法器**：最简单的时序乘法器实现
2. **阵列乘法器**：纯组合逻辑的高速乘法器
3. **Booth算法**：高效的有符号数乘法算法
4. **恢复余数除法器**：基础的迭代除法实现
5. **非恢复余数除法器**：优化的除法算法
6. **性能对比**：不同实现之间的权衡
7. **C语言映射**：理解高层代码如何映射到硬件

这些知识对于优化数值计算程序、理解编译器优化以及数字电路设计都非常重要。

---

## 参考资源

1. Patterson & Hennessy - 《计算机体系结构：量化研究方法》
2. Koren - 《Computer Arithmetic Algorithms》
3. Flynn - 《Advanced Computer Arithmetic Design》
4. IEEE 754 浮点数标准
5. Verilog HDL 数字系统设计


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
