# RISC-V 处理器完整设计 (RISC-V CPU Complete Design)

> **层级定位**: 02 Formal Semantics and Physics / 10 ISA_Machine_Code
> **主题**: 从 ISA 到 RTL 到 FPGA 验证的完整 RISC-V CPU 设计
> **难度级别**: L5 专家
> **预估学习时间**: 40-60 小时
> **对标课程**: MIT 6.175 / Berkeley CS61C / RVfpga

## 概述

RISC-V 是目前唯一开源、模块化、无专利负担的主流指令集架构。
本模块系统指导如何从 RISC-V ISA 规范出发，设计一个完整的 32/64 位处理器，涵盖数据通路设计、控制单元、流水线、冒险处理、分支预测和缓存集成，最终通过 FPGA 验证，对齐 MIT 6.175 的"构造式计算机架构"教学体系。

---

## 设计路线图

```
Phase 1: 单周期 CPU (支持 RV32I)
    │
    ▼
Phase 2: 多周期 CPU (资源共享，减少硬件)
    │
    ▼
Phase 3: 5 级流水线 CPU (提高吞吐量)
    │
    ▼
Phase 4: 流水线优化 (转发、冒险检测、分支预测)
    │
    ▼
Phase 5: 缓存集成 + 异常/中断处理
```

---

## Phase 1: 单周期 RV32I CPU

### RV32I 指令格式速查

| 类型 | 用途 | 字段 |
|:-----|:-----|:-----|
| **R-type** | 寄存器-寄存器运算 | `funct7 rs2 rs1 funct3 rd opcode` |
| **I-type** | 立即数运算/加载/跳转 | `imm[11:0] rs1 funct3 rd opcode` |
| **S-type** | 存储 | `imm[11:5] rs2 rs1 funct3 imm[4:0] opcode` |
| **B-type** | 分支 | `imm[12|10:5] rs2 rs1 funct3 imm[4:1|11] opcode` |
| **U-type** | 大立即数 (LUI/AUIPC) | `imm[31:12] rd opcode` |
| **J-type** | 跳转 (JAL) | `imm[20|10:1|11|19:12] rd opcode` |

### 数据通路设计

```
┌─────────────────────────────────────────────────────────────┐
│                        单周期数据通路                         │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  PC ──→ 指令存储器 ──→ 指令寄存器                             │
│   │                        │                                │
│   │                        ▼                                │
│   │                   控制单元                               │
│   │                   (opcode → 控制信号)                     │
│   │                        │                                │
│   ▼                        ▼                                │
│  PC+4 ←────────────────────┘                                │
│                                                             │
│  寄存器文件 (x0-x31)                                         │
│    ▲  rs1,rs2 ─┬─→ ALU (运算/地址计算)                      │
│    │            │      │                                    │
│    │            │      ├──→ 数据存储器 (Load/Store)          │
│    │            │      │         │                          │
│    │            │      │         ▼                          │
│    │            │      │    写回数据选择                     │
│    │            │      │         │                          │
│    └────────────┴──────┴─────────┘                          │
│         rd                                                    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 核心模块 Verilog 代码

```verilog
// PC 模块
module pc (
    input clk, reset,
    input [31:0] next_pc,
    output reg [31:0] pc
);
    always @(posedge clk or posedge reset)
        if (reset) pc <= 32'h0000_0000;
        else       pc <= next_pc;
endmodule

// 寄存器文件 (x0 硬连线为 0)
module regfile (
    input clk, we,
    input [4:0] rs1, rs2, rd,
    input [31:0] wd,
    output [31:0] rd1, rd2
);
    reg [31:0] regs [0:31];

    assign rd1 = (rs1 == 0) ? 0 : regs[rs1];
    assign rd2 = (rs2 == 0) ? 0 : regs[rs2];

    always @(posedge clk)
        if (we && rd != 0) regs[rd] <= wd;
endmodule

// ALU
module alu (
    input [31:0] a, b,
    input [3:0] alu_ctrl,
    output reg [31:0] result,
    output zero
);
    localparam ADD=0, SUB=1, AND=2, OR=3, XOR=4, SLL=5, SRL=6, SRA=7,
               SLT=8, SLTU=9;

    always @(*) begin
        case (alu_ctrl)
            ADD:  result = a + b;
            SUB:  result = a - b;
            AND:  result = a & b;
            OR:   result = a | b;
            XOR:  result = a ^ b;
            SLL:  result = a << b[4:0];
            SRL:  result = a >> b[4:0];
            SRA:  result = $signed(a) >>> b[4:0];
            SLT:  result = ($signed(a) < $signed(b)) ? 1 : 0;
            SLTU: result = (a < b) ? 1 : 0;
            default: result = 0;
        endcase
    end

    assign zero = (result == 0);
endmodule
```

### 控制信号真值表

| 指令 | RegWrite | ALUSrc | MemRead | MemWrite | MemtoReg | Branch | ALUOp |
|:-----|:---------|:-------|:--------|:---------|:---------|:-------|:------|
| add  | 1        | 0      | 0       | 0        | 0        | 0      | 10    |
| lw   | 1        | 1      | 1       | 0        | 1        | 0      | 00    |
| sw   | 0        | 1      | 0       | 1        | X        | 0      | 00    |
| beq  | 0        | 0      | 0       | 0        | X        | 1      | 01    |
| jal  | 1        | X      | 0       | 0        | 2        | 1      | XX    |

---

## Phase 2: 多周期 CPU

### 动机

单周期 CPU 的时钟周期由最长指令（Load）决定，硬件利用率低。多周期 CPU 将指令执行分解为多个时钟周期，共享硬件资源（ALU、存储器）。

### 5 个执行阶段

1. **取指 (IF)**: `PC → IR`
2. **译码/读寄存器 (ID)**: 解析指令，读 rs1/rs2
3. **执行 (EX)**: ALU 运算/地址计算
4. **访存 (MEM)**: 数据存储器读写
5. **写回 (WB)**: 结果写入寄存器文件

### 状态机控制

```verilog
localparam IF  = 3'd0,
           ID  = 3'd1,
           EX  = 3'd2,
           MEM = 3'd3,
           WB  = 3'd4;

always @(posedge clk)
    case (state)
        IF:  state <= ID;
        ID:  state <= EX;
        EX:  state <= is_load_store ? MEM : WB;
        MEM: state <= is_load ? WB : IF;
        WB:  state <= IF;
    endcase
```

---

## Phase 3: 5 级流水线 CPU

### 流水线寄存器

```
IF/ID:  PC, Instruction
ID/EX:  PC, RegReadData1, RegReadData2, Immediate, rs1, rs2, rd, Control
EX/MEM: ALUResult, WriteData, rd, Control
MEM/WB: ReadData/ALUResult, rd, Control
```

### Verilog 流水线框架

```verilog
module pipeline_cpu (
    input clk, reset,
    output [31:0] pc
);
    // IF 阶段
    wire [31:0] if_pc, if_pc_plus4, if_instruction;
    // ...

    // IF/ID 流水线寄存器
    reg [31:0] ifid_pc, ifid_instruction;
    always @(posedge clk) begin
        ifid_pc <= if_pc;
        ifid_instruction <= if_instruction;
    end

    // ID 阶段
    // ...

    // ID/EX 流水线寄存器
    // ...

    // EX, MEM, WB 阶段类似...
endmodule
```

---

## Phase 4: 流水线优化

### 4.1 数据冒险与转发 (Forwarding)

```
add x1, x2, x3     # WB 阶段写入 x1
sub x4, x1, x5     # EX 阶段需要 x1 → 冒险！
```

**转发逻辑**:

```verilog
// EX 阶段的转发 MUX
assign forward_a =
    (ex_mem_regwrite && ex_mem_rd != 0 && ex_mem_rd == id_ex_rs1) ? 2'b10 :
    (mem_wb_regwrite && mem_wb_rd != 0 && mem_wb_rd == id_ex_rs1) ? 2'b01 :
                                                                     2'b00;

// ALU 输入选择
assign alu_in1 = (forward_a == 2'b10) ? ex_mem_aluresult :
                 (forward_a == 2'b01) ? mem_wb_writedata :
                                        id_ex_readdata1;
```

### 4.2 控制冒险与分支预测

| 策略 | 实现 | 准确率 |
|:-----|:-----|:-------|
| **静态不跳转** | 总是预测分支不执行 | ~50% |
| **静态跳转** | 总是预测分支执行 | ~60% |
| **BTB (分支目标缓存)** | 记录历史跳转目标 | ~80% |
| **1-bit 预测器** | 记录上次是否跳转 | ~85% |
| **2-bit 饱和计数器** | 4 状态机 (强不跳/弱不跳/弱跳/强跳) | ~90% |
| **GShare/Tournament** | 全局历史 + 局部历史混合 | ~95% |

### 4.3 2-bit 饱和计数器实现

```verilog
module branch_predictor (
    input clk, update,
    input [9:0] pc_index,     // PC 低 10 位索引
    input actual_taken,
    output prediction
);
    reg [1:0] pht [0:1023];   // Pattern History Table

    // 00: 强不跳, 01: 弱不跳, 10: 弱跳, 11: 强跳
    assign prediction = pht[pc_index][1];  // MSB 为预测结果

    always @(posedge clk)
        if (update)
            if (actual_taken && pht[pc_index] != 2'b11)
                pht[pc_index] <= pht[pc_index] + 1;
            else if (!actual_taken && pht[pc_index] != 2'b00)
                pht[pc_index] <= pht[pc_index] - 1;
endmodule
```

---

## Phase 5: 缓存集成

### 数据缓存 (L1 D-Cache)

```verilog
module dcache (
    input clk,
    input [31:0] addr,
    input [31:0] write_data,
    input memread, memwrite,
    output [31:0] read_data,
    output hit
);
    // 参数: 64 行, 每行 16 字 (64B), 2-way 组相联
    localparam NUM_SETS = 32;
    localparam WAYS = 2;

    reg [31:8] tag [0:NUM_SETS-1][0:WAYS-1];   // 24-bit tag
    reg valid   [0:NUM_SETS-1][0:WAYS-1];
    reg lru     [0:NUM_SETS-1];                   // LRU bit
    reg [31:0] data [0:NUM_SETS-1][0:WAYS-1][0:15];  // 数据

    // 地址解析: [31:8] tag, [7:5] index, [4:2] word, [1:0] byte
    wire [24:0] tag_field = addr[31:8];
    wire [2:0]  set_index = addr[7:5];
    wire [3:0]  word_index = addr[4:2];

    // 命中检测
    wire way0_hit = valid[set_index][0] && tag[set_index][0] == tag_field;
    wire way1_hit = valid[set_index][1] && tag[set_index][1] == tag_field;
    assign hit = way0_hit || way1_hit;

    // 数据输出
    assign read_data = way0_hit ? data[set_index][0][word_index] :
                                   data[set_index][1][word_index];

    // ... 更新逻辑
endmodule
```

---

## Phase 6: 64 位扩展 (RV64I)

RV64I 将寄存器宽度从 32 位扩展到 64 位，并引入双字 (64位) 加载/存储和移位指令。

### RV64I 新增/修改指令

| 指令 | 格式 | 功能 | 说明 |
|:-----|:-----|:-----|:-----|
| `lwu` | I | 加载无符号字 (32→64位零扩展) | 替代RV32I的`lw`进行零扩展加载 |
| `ld` | I | 加载双字 (64位) | 地址必须8字节对齐 |
| `sd` | S | 存储双字 (64位) | 地址必须8字节对齐 |
| `addiw` | I | 加立即数 (32位结果符号扩展) | 保持32位兼容性 |
| `slliw` | I | 逻辑左移立即数 (32位) | 仅低5位有效 |
| `srliw` | I | 逻辑右移立即数 (32位) | 零扩展高位 |
| `sraiw` | I | 算术右移立即数 (32位) | 符号扩展高位 |
| `addw` | R | 32位加法 (结果符号扩展) | 用于32位模拟 |
| `subw` | R | 32位减法 | 同上 |
| `sllw` | R | 32位逻辑左移 | 同上 |
| `srlw` | R | 32位逻辑右移 | 同上 |
| `sraw` | R | 32位算术右移 | 同上 |

### 64位数据通路修改

```verilog
// RV64I 寄存器文件
module regfile64 (
    input clk,
    input [4:0] rs1, rs2, rd,
    input [63:0] writedata,
    input regwrite,
    output [63:0] readdata1, readdata2
);
    reg [63:0] registers [0:31];
    // x0 硬连线为0
    assign readdata1 = (rs1 == 0) ? 64'b0 : registers[rs1];
    assign readdata2 = (rs2 == 0) ? 64'b0 : registers[rs2];

    always @(posedge clk)
        if (regwrite && rd != 0)
            registers[rd] <= writedata;
endmodule

// ALU 64位操作
module alu64 (
    input [63:0] a, b,
    input [3:0] alucontrol,
    output [63:0] result,
    output zero
);
    wire [63:0] b_ext = alucontrol[3] ? ~b + 1 : b;  // 减法时取补码

    always @(*)
        case (alucontrol[2:0])
            3'b000: result = a + b_ext;           // ADD/SUB
            3'b001: result = a << b[5:0];         // SLL (64位移位用6位)
            3'b010: result = ($signed(a) < $signed(b)) ? 1 : 0;  // SLT
            3'b011: result = (a < b) ? 1 : 0;     // SLTU
            3'b100: result = a ^ b;               // XOR
            3'b101: result = alucontrol[3] ? $signed(a) >>> b[5:0]  // SRA
                                            : a >> b[5:0];         // SRL
            3'b110: result = a | b;               // OR
            3'b111: result = a & b;               // AND
        endcase

    assign zero = (result == 64'b0);
endmodule
```

### 32位操作兼容性

```verilog
// 32位操作结果必须符号扩展为64位
wire [31:0] addw_result = a[31:0] + b[31:0];
wire [63:0] addw_sext = {{32{addw_result[31]}}, addw_result};
```

---

## Phase 7: 乘除法单元 (M 扩展)

RISC-V M 扩展 (`rv32im` / `rv64im`) 增加整数乘除法指令。
现代处理器中，乘法通常用 3-5 周期流水线实现，除法用迭代算法 (20-40 周期)。

### M 扩展指令集

| 指令 | 格式 | 功能 | 延迟 (典型) |
|:-----|:-----|:-----|:-----------|
| `mul` | R | 乘法 (低半部) | 3-5 周期 |
| `mulh` | R | 乘法高半部 (有符号×有符号) | 3-5 周期 |
| `mulhu` | R | 乘法高半部 (无符号×无符号) | 3-5 周期 |
| `mulhsu` | R | 乘法高半部 (有符号×无符号) | 3-5 周期 |
| `div` | R | 有符号除法 | 20-40 周期 |
| `divu` | R | 无符号除法 | 20-40 周期 |
| `rem` | R | 有符号取余 | 20-40 周期 |
| `remu` | R | 无符号取余 | 20-40 周期 |

### 乘法器实现 (Booth 算法)

```verilog
// 有符号乘法器 (Booth 编码，5周期流水线)
module multiplier (
    input clk, reset,
    input [31:0] multiplicand, multiplier,
    input start,              // 开始信号
    output [63:0] product,
    output done
);
    // 状态机
    localparam IDLE = 2'b00, CALC = 2'b01, DONE = 2'b10;
    reg [1:0] state;
    reg [4:0] count;          // 计数器 (32次迭代)
    reg [63:0] product_reg;   // {high, low}
    reg [31:0] mcand;

    // Booth 算法核心
    always @(posedge clk) begin
        if (reset) begin
            state <= IDLE;
            count <= 0;
        end else case (state)
            IDLE: if (start) begin
                product_reg <= {32'b0, multiplier};
                mcand <= multiplicand;
                count <= 0;
                state <= CALC;
            end
            CALC: begin
                // 检查 product_reg[1:0] (Booth 两位编码)
                case (product_reg[1:0])
                    2'b01: product_reg[63:32] <= product_reg[63:32] + mcand;
                    2'b10: product_reg[63:32] <= product_reg[63:32] - mcand;
                endcase
                // 算术右移
                product_reg <= $signed(product_reg) >>> 1;
                count <= count + 1;
                if (count == 31) state <= DONE;
            end
            DONE: state <= IDLE;
        endcase
    end

    assign product = product_reg;
    assign done = (state == DONE);
endmodule
```

### 除法器实现 (恢复余数算法)

```verilog
// 无符号除法器 (迭代算法)
module divider (
    input clk, reset,
    input [31:0] dividend, divisor,
    input start,
    output [31:0] quotient, remainder,
    output done
);
    localparam IDLE = 2'b00, CALC = 2'b01, DONE = 2'b10;
    reg [1:0] state;
    reg [4:0] count;
    reg [63:0] remainder_reg;  // {rem, quot}
    reg [31:0] divisor_reg;

    always @(posedge clk) begin
        if (reset) begin
            state <= IDLE;
        end else case (state)
            IDLE: if (start) begin
                remainder_reg <= {32'b0, dividend};
                divisor_reg <= divisor;
                count <= 0;
                state <= CALC;
            end
            CALC: begin
                // 左移，试减
                remainder_reg <= remainder_reg << 1;
                if (remainder_reg[63:32] >= divisor_reg) begin
                    remainder_reg[63:32] <= remainder_reg[63:32] - divisor_reg;
                    remainder_reg[0] <= 1'b1;  // 商位置1
                end
                count <= count + 1;
                if (count == 31) state <= DONE;
            end
            DONE: state <= IDLE;
        endcase
    end

    assign quotient = remainder_reg[31:0];
    assign remainder = remainder_reg[63:32];
    assign done = (state == DONE);
endmodule
```

### 流水线集成考虑

```verilog
// 在流水线中，乘除法需要多周期完成
// 方案1: 阻塞流水线 (简单但性能差)
// 方案2: 记分板/ Tomasulo 算法 (高性能乱序执行)

// 简单阻塞方案
always @(*)
    case (opcode)
        // ... 其他指令
        OP_MUL:  stall = ~mul_done;   // 等待乘法完成
        OP_DIV:  stall = ~div_done;   // 等待除法完成
    endcase
```

---

## 验证与测试

### RISC-V 指令测试 (riscv-tests)

```bash
# 使用官方 RISC-V 测试套件
git clone https://github.com/riscv/riscv-tests
cd riscv-tests
./configure --prefix=$RISCV/target
make
make install

# 运行测试
spike +signature=ref.sig pk hello
# 对比你的 CPU 输出与 Spike（参考模拟器）
```

### FPGA 验证流程

```bash
# 1. 综合
vivado -mode batch -source synth.tcl

# 2. 生成比特流
write_bitstream -force riscv_cpu.bit

# 3. 上板测试
open_hw_manager
connect_hw_server
open_hw_target
set_property PROGRAM.FILE {riscv_cpu.bit} [get_hw_devices xc7a35t_0]
program_hw_devices
```

---

## 权威参考

- **RISC-V ISA Spec Vol.1**: 非特权指令集架构
- **RISC-V ISA Spec Vol.2**: 特权架构（异常/中断/虚拟内存）
- **"Computer Organization and Design RISC-V Edition"** (Patterson & Hennessy)
- **MIT 6.175**: Constructive Computer Architecture (BSV/Bluespec)
- **Berkeley CS61C**: RISC-V CPU project in Logisim/Verilog
- **RVfpga**: RISC-V + FPGA 完整课程 (Imagination Technologies)

---

*最后更新: 2026-05-10*
