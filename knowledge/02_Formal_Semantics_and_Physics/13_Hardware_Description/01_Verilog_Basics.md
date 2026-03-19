# Verilog硬件描述语言基础

## 1. 概述

### 1.1 什么是Verilog？

Verilog是硬件描述语言(HDL)，用于描述数字电路：

```
Verilog能描述:
├── 组合逻辑 (Combinational Logic)
├── 时序逻辑 (Sequential Logic)
├── 有限状态机 (FSM)
├── 处理器 (CPU)
└── 完整SoC系统

应用场景:
├── FPGA开发
├── ASIC设计
├── 硬件验证
└── 数字电路教学
```

### 1.2 与C语言的对比

| 特性 | C语言 | Verilog |
|------|-------|---------|
| **执行** | 顺序执行 | 并行执行 |
| **时间** | 运行时 | 仿真时间 |
| **变量** | 存储值 | 表示导线/寄存器 |
| **编译** | 编译为机器码 | 综合为电路网表 |
| **调试** | 断点调试 | 波形查看 |

---

## 2. 基本语法

### 2.1 模块定义

```verilog
// 模块 = 电路的封装单元
module adder (
    input  [7:0] a,      // 8位输入a
    input  [7:0] b,      // 8位输入b
    input        cin,    // 进位输入
    output [7:0] sum,    // 8位和输出
    output       cout    // 进位输出
);

    // 内部逻辑
    assign {cout, sum} = a + b + cin;

endmodule
```

### 2.2 数据类型

```verilog
// 线网类型 (组合逻辑)
wire        w1;        // 1位线网
wire [7:0]  bus;       // 8位总线
wire [31:0] data;      // 32位数据

// 寄存器类型 (时序逻辑)
reg         r1;        // 1位寄存器
reg [7:0]   counter;   // 8位计数器
reg [3:0]   state;     // 状态寄存器

// 整数 (仿真用)
integer     i;

// 常量
parameter   WIDTH = 8;
localparam  IDLE  = 2'b00;  // 2位二进制00
```

### 2.3 运算符

```verilog
// 算术运算符
assign sum = a + b;     // 加法
assign diff = a - b;    // 减法
assign prod = a * b;    // 乘法
assign quot = a / b;    // 除法
assign rem = a % b;     // 取模

// 位运算符
assign y = ~a;          // 按位非
assign y = a & b;       // 按位与
assign y = a | b;       // 按位或
assign y = a ^ b;       // 按位异或

// 逻辑运算符
assign y = !a;          // 逻辑非
assign y = a && b;      // 逻辑与
assign y = a || b;      // 逻辑或

// 关系运算符
assign eq = (a == b);   // 等于
assign ne = (a != b);   // 不等于
assign lt = (a < b);    // 小于
assign gt = (a > b);    // 大于

// 移位运算符
assign y = a << 2;      // 左移2位
assign y = a >> 2;      // 右移2位

// 拼接
assign y = {a, b};              // {a,b}
assign y = {4{a}};              // {a,a,a,a} 复制
assign y = {a[3:0], b[3:0]};    // 位选择
```

---

## 3. 组合逻辑

### 3.1 连续赋值 (assign)

```verilog
// 组合逻辑使用assign
module mux2to1 (
    input  [7:0] a,
    input  [7:0] b,
    input        sel,
    output [7:0] y
);
    // 2选1多路选择器
    assign y = sel ? b : a;

endmodule

// 译码器
module decoder3to8 (
    input  [2:0] in,
    output [7:0] out
);
    // 3-8译码器
    assign out = (in == 3'b000) ? 8'b00000001 :
                 (in == 3'b001) ? 8'b00000010 :
                 (in == 3'b010) ? 8'b00000100 :
                 (in == 3'b011) ? 8'b00001000 :
                 (in == 3'b100) ? 8'b00010000 :
                 (in == 3'b101) ? 8'b00100000 :
                 (in == 3'b110) ? 8'b01000000 :
                                  8'b10000000;

endmodule
```

### 3.2 always组合块

```verilog
module alu (
    input  [31:0] a, b,
    input  [2:0]  op,
    output reg [31:0] result,
    output reg        zero
);
    // 组合always块
    always @(*) begin
        case (op)
            3'b000: result = a + b;      // ADD
            3'b001: result = a - b;      // SUB
            3'b010: result = a & b;      // AND
            3'b011: result = a | b;      // OR
            3'b100: result = a ^ b;      // XOR
            3'b101: result = a << b[4:0]; // SLL
            3'b110: result = a >> b[4:0]; // SRL
            3'b111: result = ($signed(a) >>> b[4:0]); // SRA
            default: result = 32'b0;
        endcase

        zero = (result == 32'b0);
    end

endmodule
```

---

## 4. 时序逻辑

### 4.1 触发器

```verilog
// D触发器
module d_ff (
    input        clk,
    input        rst,    // 异步复位
    input        d,
    output reg   q
);
    always @(posedge clk or posedge rst) begin
        if (rst)
            q <= 1'b0;
        else
            q <= d;
    end

endmodule

// 带使能的D触发器
module d_ff_en (
    input        clk,
    input        rst,
    input        en,     // 使能
    input        d,
    output reg   q
);
    always @(posedge clk or posedge rst) begin
        if (rst)
            q <= 1'b0;
        else if (en)
            q <= d;
    end

endmodule
```

### 4.2 计数器

```verilog
module counter (
    input        clk,
    input        rst,
    input        en,
    output reg [7:0] count
);
    always @(posedge clk or posedge rst) begin
        if (rst)
            count <= 8'b0;
        else if (en)
            count <= count + 1'b1;
    end

endmodule

// 可加载计数器
module loadable_counter (
    input        clk,
    input        rst,
    input        load,
    input  [7:0] load_val,
    output reg [7:0] count
);
    always @(posedge clk or posedge rst) begin
        if (rst)
            count <= 8'b0;
        else if (load)
            count <= load_val;
        else
            count <= count + 1'b1;
    end

endmodule
```

### 4.3 移位寄存器

```verilog
module shift_reg (
    input        clk,
    input        rst,
    input        shift_en,
    input        serial_in,
    output       serial_out,
    output [7:0] parallel_out
);
    reg [7:0] reg_data;

    always @(posedge clk or posedge rst) begin
        if (rst)
            reg_data <= 8'b0;
        else if (shift_en)
            reg_data <= {serial_in, reg_data[7:1]};  // 右移
    end

    assign serial_out = reg_data[0];
    assign parallel_out = reg_data;

endmodule
```

---

## 5. 有限状态机 (FSM)

### 5.1 Moore型FSM

```verilog
module traffic_light (
    input        clk,
    input        rst,
    input        car_detected,
    output reg [2:0] light    // 3位: 红、黄、绿
);
    // 状态编码
    localparam GREEN  = 2'b00;
    localparam YELLOW = 2'b01;
    localparam RED    = 2'b10;

    reg [1:0] state, next_state;
    reg [4:0] timer;  // 5位计时器 (0-31)

    // 状态寄存器 (时序)
    always @(posedge clk or posedge rst) begin
        if (rst)
            state <= GREEN;
        else
            state <= next_state;
    end

    // 下一状态逻辑 (组合)
    always @(*) begin
        next_state = state;  // 默认保持

        case (state)
            GREEN:  if (car_detected) next_state = YELLOW;
            YELLOW: if (timer == 5)   next_state = RED;
            RED:    if (timer == 20)  next_state = GREEN;
        endcase
    end

    // 输出逻辑 (Moore: 仅依赖状态)
    always @(*) begin
        case (state)
            GREEN:  light = 3'b001;  // 绿
            YELLOW: light = 3'b010;  // 黄
            RED:    light = 3'b100;  // 红
        endcase
    end

    // 计时器
    always @(posedge clk or posedge rst) begin
        if (rst)
            timer <= 5'b0;
        else if (state != next_state)
            timer <= 5'b0;  // 状态切换时清零
        else
            timer <= timer + 1'b1;
    end

endmodule
```

---

## 6. 存储器

### 6.1 单端口RAM

```verilog
module single_port_ram (
    input        clk,
    input        we,           // 写使能
    input  [7:0] addr,         // 地址
    input  [31:0] din,         // 数据输入
    output [31:0] dout         // 数据输出
);
    reg [31:0] mem [0:255];    // 256 x 32位存储器

    always @(posedge clk) begin
        if (we)
            mem[addr] <= din;   // 写操作
    end

    assign dout = mem[addr];    // 读操作 (异步)

endmodule
```

### 6.2 双端口RAM

```verilog
module dual_port_ram (
    input        clk,
    // 端口A
    input        we_a,
    input  [7:0] addr_a,
    input  [31:0] din_a,
    output [31:0] dout_a,
    // 端口B
    input        we_b,
    input  [7:0] addr_b,
    input  [31:0] din_b,
    output [31:0] dout_b
);
    reg [31:0] mem [0:255];

    // 端口A
    always @(posedge clk) begin
        if (we_a)
            mem[addr_a] <= din_a;
    end
    assign dout_a = mem[addr_a];

    // 端口B
    always @(posedge clk) begin
        if (we_b)
            mem[addr_b] <= din_b;
    end
    assign dout_b = mem[addr_b];

endmodule
```

---

## 7. 测试平台 (Testbench)

```verilog
`timescale 1ns/1ps

module tb_adder;
    // 信号声明
    reg  [7:0] a, b;
    reg        cin;
    wire [7:0] sum;
    wire       cout;

    // 实例化被测模块
    adder uut (
        .a(a),
        .b(b),
        .cin(cin),
        .sum(sum),
        .cout(cout)
    );

    // 测试激励
    initial begin
        // 初始化
        a = 0; b = 0; cin = 0;

        #10 a = 8'd10; b = 8'd20; cin = 0;
        #10 a = 8'd255; b = 8'd1; cin = 0;
        #10 a = 8'd255; b = 8'd255; cin = 1;

        #10 $finish;
    end

    // 监视信号变化
    initial begin
        $monitor("Time=%0t a=%d b=%d cin=%b sum=%d cout=%b",
                 $time, a, b, cin, sum, cout);
    end

endmodule
```

---

## 8. C与Verilog的协作

### 8.1 高层次综合 (HLS) 概念

```c
// C算法
int sum_array(int arr[100]) {
    int sum = 0;
    for (int i = 0; i < 100; i++) {
        sum += arr[i];
    }
    return sum;
}

// 对应的Verilog概念
// - 循环展开为并行加法树
// - 数组映射为存储器
// - 函数映射为电路模块
```

### 8.2 软硬件协同设计

```
SoC架构
├── 处理器 (运行C代码)
│   └── 主控程序
├── 硬件加速器 (Verilog)
│   ├── 图像处理模块
│   └── 加密引擎
└── 互连总线
    └── AXI/AXI-Lite
```

---

## 9. 学习资源

| 资源 | 链接 |
|------|------|
| Verilog标准 | IEEE 1364 |
| 模拟器 | Icarus Verilog, ModelSim |
| FPGA工具 | Vivado, Quartus |
| 在线仿真 | EDA Playground |

---

**文档版本**: 1.0
**创建日期**: 2026-03-19


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
