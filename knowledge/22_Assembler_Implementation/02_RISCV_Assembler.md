# RISC-V汇编器实现

> **层级定位**: 22 Assembler Implementation / 进阶层
> **前置知识**: [Hack汇编器](./01_Hack_Assembler.md)
> **后续延伸**: [C到汇编：数据表示](./03_C_to_Assembly_Data.md) → [C到汇编：控制流](./04_C_to_Assembly_Control.md)
> **横向关联**: [RISC-V CPU实现](../02_Formal_Semantics_and_Physics/10_ISA_Machine_Code/08_RISCV_CPU_Complete/README.md)
> **预计学习时间**: 2周
> **实践验证**: [riscv_assembler.c](../../examples/assembler/riscv_assembler.c)

---

## 核心洞察

```
RISC-V汇编器是连接高级语言与真实硬件的关键桥梁。
与Hack汇编器相比，RISC-V汇编器展现出现代处理器的复杂性：

1. 多种指令格式（R/I/S/B/U/J型）
2. 复杂的立即数编码和扩展
3. 伪指令的展开处理
4. 与链接器的协作（重定位）

理解RISC-V汇编器，就是理解现代编译器后端的基础。
```

---

## 1. RISC-V指令格式详解

### 1.1 指令格式概览

RISC-V基础指令集（RV32I）定义了六种基本指令格式：

```
┌─────────────────────────────────────────────────────────────┐
│  31  30  29  28  27  26  25  24  23  22  21  20  19  18  17  │
├─────────────────────────────────────────────────────────────┤
│ funct7 │      rs2    │      rs1    │ funct3 │      rd      │ R型
├────────┴─────────────┴─────────────┴────────┴──────────────┤
│           imm[11:0]          │      rs1    │ funct3 │      rd      │ I型
├──────────────────────────────┴─────────────┴────────┴──────────────┤
│  imm[11:5]   │      rs2    │      rs1    │ funct3 │  imm[4:0]  │ S型
├──────────────┴─────────────┴─────────────┴────────┴──────────────┤
│ imm[12|10:5] │      rs2    │      rs1    │ funct3 │imm[4:1|11]│ B型
├──────────────┴───────────────────────────┴────────┴──────────────┤
│                    imm[31:12]                    │      rd      │ U型
├──────────────────────────────────────────────────┴──────────────┤
│          imm[20|10:1|11|19:12]                   │      rd      │ J型
└─────────────────────────────────────────────────────────────────┘
      31                                                    0
```

### 1.2 各类型指令详解

#### R型（寄存器-寄存器操作）

```
31      25 24      20 19      15 14  12 11       7 6       0
┌─────────┬─────────┬─────────┬──────┬─────────┬─────────┐
│  funct7 │   rs2   │   rs1   │funct3│   rd    │ opcode  │
└─────────┴─────────┴─────────┴──────┴─────────┴─────────┘
   7 bits   5 bits    5 bits   3 bits  5 bits   7 bits

示例: add x1, x2, x3
  - funct7 = 0000000
  - rs2 = 00011 (x3)
  - rs1 = 00010 (x2)
  - funct3 = 000
  - rd = 00001 (x1)
  - opcode = 0110011

二进制: 0000000 00011 00010 000 00001 0110011
        = 0x003100B3
```

#### I型（立即数操作）

```
31                    20 19      15 14  12 11       7 6       0
┌─────────────────────────┬─────────┬──────┬─────────┬─────────┐
│      imm[11:0]          │   rs1   │funct3│   rd    │ opcode  │
└─────────────────────────┴─────────┴──────┴─────────┴─────────┘

示例: addi x1, x2, -5
  - imm[11:0] = 111111111011 (-5的12位补码)
  - rs1 = 00010 (x2)
  - funct3 = 000
  - rd = 00001 (x1)
  - opcode = 0010011

二进制: 111111111011 00010 000 00001 0010011
        = 0xFFB10093

注意: 12位立即数是有符号的，范围 -2048 到 +2047
```

#### S型（存储指令）

```
31      25 24      20 19      15 14  12 11       7 6       0
┌─────────┬─────────┬─────────┬──────┬─────────┬─────────┐
│imm[11:5]│   rs2   │   rs1   │funct3│imm[4:0] │ opcode  │
└─────────┴─────────┴─────────┴──────┴─────────┴─────────┘

示例: sw x2, 8(x3)
  - imm[11:5] = 0000000
  - imm[4:0] = 01000
  - rs2 = 00010 (x2，要存储的值)
  - rs1 = 00011 (x3，基地址)
  - funct3 = 010
  - opcode = 0100011

二进制: 0000000 00010 00011 010 01000 0100011
        = 0x0221A123

关键洞察: 立即数被分割！编码需要特别注意位域分布
```

#### B型（分支指令）

```
31      25 24      20 19      15 14  12 11       7 6       0
┌─────────┬─────────┬─────────┬──────┬─────────┬─────────┐
│imm[12|10│   rs2   │   rs1   │funct3│imm[4:1] │ opcode  │
│  :5]    │         │         │      │  [11]   │         │
└─────────┴─────────┴─────────┴──────┴─────────┴─────────┘

B型立即数编码（位序混乱，这是为了硬件简化）：
  inst[31]    = imm[12]
  inst[30:25] = imm[10:5]
  inst[11:8]  = imm[4:1]
  inst[7]     = imm[11]

示例: beq x1, x2, loop (假设loop在当前指令-12字节处)
  偏移 = -12 = 0xFF4，但B型使用字节偏移/2
  实际编码值 = -12 / 2 = -6

  imm[12:0] = 1111111111010 (-6的13位补码，左移1位)
  imm[12] = 1, imm[10:5] = 111111, imm[4:1] = 1101, imm[11] = 1

注意: 分支目标必须2字节对齐，imm[0]始终为0（隐含）
```

### 1.3 立即数编码总结表

| 类型 | 立即数位 | 编码位置 | 符号扩展 | 用途 |
|:-----|:---------|:---------|:---------|:-----|
| I型 | [11:0] | [31:20] | 是 | 算术、加载、JALR |
| S型 | [11:0] | [31:25], [11:7] | 是 | 存储 |
| B型 | [12:1] | [31], [7], [30:25], [11:8] | 是 | 分支 |
| U型 | [31:12] | [31:12] | 否（左移12位）| LUI, AUIPC |
| J型 | [20:1] | [31], [19:12], [20], [30:21] | 是 | JAL |

```c
// 立即数提取函数（用于反汇编或模拟器）
int32_t extract_i_imm(uint32_t inst) {
    return (int32_t)(inst & 0xFFF00000) >> 20;
}

int32_t extract_s_imm(uint32_t inst) {
    int32_t imm = ((inst & 0xFE000000) >> 20) | ((inst & 0x00000F80) >> 7);
    return (imm << 20) >> 20;  // 符号扩展
}

int32_t extract_b_imm(uint32_t inst) {
    int32_t imm = ((inst & 0x80000000) >> 19) |
                  ((inst & 0x7E000000) >> 20) |
                  ((inst & 0x00000F00) >> 7)  |
                  ((inst & 0x00000080) << 4);
    return (imm << 19) >> 19;  // 符号扩展
}
```

---

## 2. 汇编器实现架构

### 2.1 两遍扫描设计

```
第一遍（First Pass）:
┌─────────────────────────────────────────────────────────────┐
│ 输入: 汇编源文件 (.s)                                        │
│   ↓                                                         │
│ 词法分析 → 语法分析                                          │
│   ↓                                                         │
│ 构建符号表（标签 → 地址映射）                                 │
│   - 记录每个标签对应的地址                                    │
│   - 分配变量到数据段                                          │
│   - 处理 .equ 等符号定义                                      │
│   ↓                                                         │
│ 计算指令地址（用于计算分支偏移）                              │
└─────────────────────────────────────────────────────────────┘

第二遍（Second Pass）:
┌─────────────────────────────────────────────────────────────┐
│ 输入: 清洗后的指令 + 完整符号表                               │
│   ↓                                                         │
│ 指令编码                                                     │
│   - 解析操作码                                               │
│   - 查找寄存器编号                                           │
│   - 计算立即数（含符号扩展检查）                              │
│   - 处理伪指令展开                                           │
│   ↓                                                         │
│ 生成目标代码                                                 │
│   - 输出机器码                                               │
│   - 生成重定位信息（如果需要链接）                            │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 核心数据结构

```c
// 符号表条目
typedef struct {
    char *name;           // 符号名称
    uint32_t address;     // 符号地址
    enum {
        SYM_LABEL,        // 代码标签
        SYM_VARIABLE,     // 数据变量
        SYM_CONSTANT,     // .equ 定义的常量
        SYM_EXTERN        // 外部符号
    } type;
    int section;          // 所属段 (.text/.data/.bss)
} Symbol;

// 指令编码表条目
typedef struct {
    char *mnemonic;       // 汇编助记符
    uint8_t opcode;       // 操作码
    uint8_t funct3;       // funct3字段
    uint8_t funct7;       // funct7字段（R型）
    enum {
        TYPE_R, TYPE_I, TYPE_S, TYPE_B, TYPE_U, TYPE_J,
        TYPE_PSEUDO       // 伪指令
    } type;
} InstructionInfo;

// 寄存器名称映射
typedef struct {
    char *name;
    uint8_t number;
} RegisterInfo;

static RegisterInfo reg_table[] = {
    {"x0", 0}, {"zero", 0},
    {"x1", 1}, {"ra", 1},
    {"x2", 2}, {"sp", 2},
    {"x3", 3}, {"gp", 3},
    {"x4", 4}, {"tp", 4},
    {"x5", 5}, {"t0", 5},
    // ... 更多寄存器
    {NULL, 0}
};
```

### 2.3 指令编码表

```c
static InstructionInfo inst_table[] = {
    // R型指令
    {"add",  0x33, 0x0, 0x00, TYPE_R},
    {"sub",  0x33, 0x0, 0x20, TYPE_R},
    {"sll",  0x33, 0x1, 0x00, TYPE_R},
    {"slt",  0x33, 0x2, 0x00, TYPE_R},
    {"xor",  0x33, 0x4, 0x00, TYPE_R},
    {"srl",  0x33, 0x5, 0x00, TYPE_R},
    {"sra",  0x33, 0x5, 0x20, TYPE_R},
    {"or",   0x33, 0x6, 0x00, TYPE_R},
    {"and",  0x33, 0x7, 0x00, TYPE_R},

    // I型指令 - 算术
    {"addi", 0x13, 0x0, 0x00, TYPE_I},
    {"slti", 0x13, 0x2, 0x00, TYPE_I},
    {"xori", 0x13, 0x4, 0x00, TYPE_I},
    {"ori",  0x13, 0x6, 0x00, TYPE_I},
    {"andi", 0x13, 0x7, 0x00, TYPE_I},

    // I型指令 - 加载
    {"lb",   0x03, 0x0, 0x00, TYPE_I},
    {"lh",   0x03, 0x1, 0x00, TYPE_I},
    {"lw",   0x03, 0x2, 0x00, TYPE_I},
    {"lbu",  0x03, 0x4, 0x00, TYPE_I},
    {"lhu",  0x03, 0x5, 0x00, TYPE_I},

    // S型指令 - 存储
    {"sb",   0x23, 0x0, 0x00, TYPE_S},
    {"sh",   0x23, 0x1, 0x00, TYPE_S},
    {"sw",   0x23, 0x2, 0x00, TYPE_S},

    // B型指令 - 分支
    {"beq",  0x63, 0x0, 0x00, TYPE_B},
    {"bne",  0x63, 0x1, 0x00, TYPE_B},
    {"blt",  0x63, 0x4, 0x00, TYPE_B},
    {"bge",  0x63, 0x5, 0x00, TYPE_B},
    {"bltu", 0x63, 0x6, 0x00, TYPE_B},
    {"bgeu", 0x63, 0x7, 0x00, TYPE_B},

    // U型指令
    {"lui",  0x37, 0x0, 0x00, TYPE_U},
    {"auipc",0x17, 0x0, 0x00, TYPE_U},

    // J型指令
    {"jal",  0x6F, 0x0, 0x00, TYPE_J},
    {"jalr", 0x67, 0x0, 0x00, TYPE_I},

    // 伪指令标记
    {"li",   0x00, 0x0, 0x00, TYPE_PSEUDO},
    {"la",   0x00, 0x0, 0x00, TYPE_PSEUDO},
    {"mv",   0x00, 0x0, 0x00, TYPE_PSEUDO},
    {"nop",  0x00, 0x0, 0x00, TYPE_PSEUDO},

    {NULL, 0, 0, 0, 0}
};
```

---

## 3. 伪指令处理

### 3.1 常见伪指令展开

```
伪指令是汇编器提供的语法糖，简化汇编编程。
它们不对应真实的机器指令，而是展开为等效的指令序列。

li（Load Immediate）展开:
─────────────────────────────────────
li rd, immediate

如果 -2048 <= immediate < 2048:
    展开为: addi rd, x0, immediate

否则（32位立即数）:
    展开为:
        lui  rd, hi20(immediate)
        addi rd, rd, lo12(immediate)

示例:
    li x1, 0x12345678
展开为:
    lui  x1, 0x12345      # 加载高20位
    addi x1, x1, 0x678    # 添加低12位

la（Load Address）展开:
─────────────────────────────────────
la rd, symbol

如果 symbol 在 ±1MB 内（使用AUIPC+ADDI）:
    auipc rd, delta[31:12] + delta[11]
    addi  rd, rd, delta[11:0]

否则（需要重定位）:
    生成重定位条目，由链接器处理

mv（Move）展开:
─────────────────────────────────────
mv rd, rs
展开为: addi rd, rs, 0

nop（No Operation）展开:
─────────────────────────────────────
nop
展开为: addi x0, x0, 0
```

### 3.2 伪指令展开实现

```c
// 伪指令展开函数
int expand_pseudo(Instruction *inst, uint32_t *output, int *count) {
    if (strcmp(inst->mnemonic, "li") == 0) {
        int32_t imm = inst->immediate;

        if (imm >= -2048 && imm < 2048) {
            // 适合I型立即数
            output[0] = encode_i_type(0x13, 0, 0, inst->rd, imm); // addi
            *count = 1;
        } else {
            // 需要两条指令
            int32_t hi = (imm >> 12) + ((imm >> 11) & 1); // 调整符号
            int32_t lo = imm & 0xFFF;

            output[0] = encode_u_type(0x37, inst->rd, hi << 12);  // lui
            output[1] = encode_i_type(0x13, 0, inst->rd, inst->rd, lo); // addi
            *count = 2;
        }
        return 0;
    }

    if (strcmp(inst->mnemonic, "mv") == 0) {
        output[0] = encode_i_type(0x13, 0, inst->rs1, inst->rd, 0); // addi
        *count = 1;
        return 0;
    }

    if (strcmp(inst->mnemonic, "nop") == 0) {
        output[0] = encode_i_type(0x13, 0, 0, 0, 0); // addi x0, x0, 0
        *count = 1;
        return 0;
    }

    if (strcmp(inst->mnemonic, "la") == 0) {
        // 假设符号地址已在符号表中
        Symbol *sym = lookup_symbol(inst->symbol);
        if (!sym) return -1;

        uint32_t addr = sym->address;
        uint32_t pc = inst->pc;
        int32_t delta = addr - pc;

        // AUIPC + ADDI 序列
        int32_t hi = ((delta + 0x800) >> 12) & 0xFFFFF;
        int32_t lo = delta & 0xFFF;

        output[0] = encode_u_type(0x17, inst->rd, hi << 12);      // auipc
        output[1] = encode_i_type(0x13, 0, inst->rd, inst->rd, lo); // addi
        *count = 2;
        return 0;
    }

    return -1; // 不是伪指令
}
```

---

## 4. 符号解析与重定位

### 4.1 符号解析流程

```
符号解析发生在两个阶段：

第一遍 - 定义收集:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
.text
    addi sp, sp, -16      # PC = 0x1000
main:                       # 符号 main = 0x1004
    sw   ra, 12(sp)
    call foo                # 需要解析 foo

.data
buffer: .space 100          # 符号 buffer = 0x2000

符号表构建:
┌─────────┬──────────┬──────────┐
│ 符号    │ 地址     │ 类型     │
├─────────┼──────────┼──────────┤
│ main    │ 0x1004   │ LABEL    │
│ buffer  │ 0x2000   │ VARIABLE │
│ foo     │ ???      │ EXTERN   │  ← 未定义
└─────────┴──────────┴──────────┘

第二遍 - 引用解析:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
call foo → 需要计算偏移量

如果 foo 在符号表中:
    offset = foo_address - current_pc
    编码 jal 指令

如果 foo 是外部符号:
    生成重定位条目
    暂时用占位符编码
```

### 4.2 重定位条目格式

```c
// ELF重定位条目（简化版）
typedef struct {
    uint32_t offset;      // 需要重定位的位置
    uint32_t info;        // 符号表索引 + 重定位类型
    int32_t  addend;      // 加数（用于RELA格式）
} RelocationEntry;

// RISC-V重定位类型（部分）
enum {
    R_RISCV_NONE,         // 无操作
    R_RISCV_32,           // 32位绝对地址
    R_RISCV_64,           // 64位绝对地址
    R_RISCV_BRANCH,       // 12位分支偏移
    R_RISCV_JAL,          // 20位JAL偏移
    R_RISCV_CALL,         // PLT调用（AUIPC+JALR对）
    R_RISCV_HI20,         // 高20位（LUI/AUIPC）
    R_RISCV_LO12_I,       // 低12位I型
    R_RISCV_LO12_S,       // 低12位S型
    R_RISCV_PCREL_HI20,   // PC相对高20位
    R_RISCV_PCREL_LO12_I, // PC相对低12位I型
};

// 重定位应用示例
void apply_relocation(uint8_t *code, RelocationEntry *rel,
                      Symbol *sym, uint32_t section_addr) {
    uint32_t target = sym->address;
    uint32_t pc = section_addr + rel->offset;
    uint32_t *inst = (uint32_t *)(code + rel->offset);

    switch (rel->type) {
        case R_RISCV_BRANCH: {
            int32_t offset = target - pc;
            *inst = encode_b_type_with_offset(*inst, offset);
            break;
        }
        case R_RISCV_JAL: {
            int32_t offset = target - pc;
            *inst = encode_j_type_with_offset(*inst, offset);
            break;
        }
        case R_RISCV_HI20: {
            int32_t value = target + rel->addend;
            *inst = (*inst & 0xFFF) | (((value >> 12) + ((value >> 11) & 1)) << 12);
            break;
        }
        // ... 更多类型
    }
}
```

---

## 5. 与RISC-V CPU实现的连接

### 5.1 从汇编器到CPU的完整流程

```
┌──────────────────────────────────────────────────────────────────────┐
│                        软件开发视角                                   │
├──────────────────────────────────────────────────────────────────────┤
│ C源代码                                                              │
│   ↓                                                                  │
│ 编译器 (gcc/clang)                                                    │
│   ↓                                                                  │
│ 汇编代码 (.s) ───────→ RISC-V汇编器 ──────→ 目标文件 (.o)              │
│   [可读的指令]          [本模块]              [机器码 + 重定位信息]    │
│   ↓                                                                  │
│ 链接器 (ld)                                                           │
│   ↓                                                                  │
│ 可执行文件                                                           │
└──────────────────────────────────────────────────────────────────────┘
                           ↓
┌──────────────────────────────────────────────────────────────────────┐
│                        硬件执行视角                                   │
├──────────────────────────────────────────────────────────────────────┤
│ 取指 (Fetch)                                                         │
│   ↓                                                                  │
│ 译码 (Decode) ←────── 理解汇编器生成的编码格式                       │
│   ↓                                                                  │
│ 执行 (Execute)                                                       │
│   ↓                                                                  │
│ 访存 (Memory) ←────── 对应load/store指令编码                         │
│   ↓                                                                  │
│ 写回 (Writeback)                                                     │
└──────────────────────────────────────────────────────────────────────┘
```

### 5.2 与CPU实现的对应关系

| 汇编器组件 | CPU对应部分 | 连接说明 |
|:-----------|:------------|:---------|
| 指令编码函数 | 译码器 | 汇编器的encode与CPU的decode是互逆操作 |
| 符号地址计算 | PC寄存器 | 分支偏移计算使用相同的PC-relative逻辑 |
| 立即数编码 | 立即数扩展单元 | 符号扩展逻辑必须一致 |
| 伪指令展开 | 微码ROM | 都是将高层语义展开为微操作序列 |
| 重定位 | MMU/链接器支持 | 运行时地址转换的静态等价 |

```c
// 译码器中的对应代码（来自您的RISC-V CPU实现）
// 对比汇编器的编码，可以理解编解码的对称性

typedef struct {
    uint8_t opcode;
    uint8_t rd, rs1, rs2;
    uint8_t funct3, funct7;
    int32_t imm;
    uint8_t type;
} DecodedInst;

DecodedInst decode(uint32_t inst) {
    DecodedInst d;
    d.opcode = inst & 0x7F;
    d.rd = (inst >> 7) & 0x1F;
    d.funct3 = (inst >> 12) & 0x7;
    d.rs1 = (inst >> 15) & 0x1F;
    d.rs2 = (inst >> 20) & 0x1F;
    d.funct7 = (inst >> 25) & 0x7F;

    // 根据opcode确定类型并提取立即数
    switch (d.opcode) {
        case 0x13: case 0x03: case 0x67: // I型
            d.type = TYPE_I;
            d.imm = (int32_t)inst >> 20;  // 符号扩展
            break;
        case 0x23: // S型
            d.type = TYPE_S;
            d.imm = ((inst >> 25) << 5) | ((inst >> 7) & 0x1F);
            d.imm = (d.imm << 20) >> 20;  // 符号扩展
            break;
        // ... 其他类型
    }
    return d;
}
```

---

## 6. 完整汇编器代码框架

```c
// riscv_assembler.c - 简化版RISC-V汇编器

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#define MAX_LINE 256
#define MAX_SYMBOLS 1024
#define MAX_CODE 65536

// ============ 数据结构 ============
typedef struct {
    char name[64];
    uint32_t address;
    int type;  // 0=label, 1=variable
} Symbol;

Symbol symtab[MAX_SYMBOLS];
int sym_count = 0;
uint32_t current_addr = 0;
int current_section = 0;  // 0=.text, 1=.data

// ============ 第一遍：构建符号表 ============
void first_pass(FILE *fp) {
    char line[MAX_LINE];
    current_addr = 0x1000;  // 代码段起始地址

    while (fgets(line, sizeof(line), fp)) {
        // 去除注释
        char *comment = strchr(line, '#');
        if (comment) *comment = '\0';

        // 检查标签
        char *colon = strchr(line, ':');
        if (colon) {
            *colon = '\0';
            // 添加符号
            strcpy(symtab[sym_count].name, line);
            symtab[sym_count].address = current_addr;
            symtab[sym_count].type = 0;
            sym_count++;
        }

        // 检查伪指令
        if (strstr(line, ".text")) {
            current_section = 0;
            current_addr = 0x1000;
        } else if (strstr(line, ".data")) {
            current_section = 1;
            current_addr = 0x2000;
        }

        // 粗略计算指令地址（实际需要更复杂的解析）
        // 简化：假设每行一条指令
        if (strlen(line) > 1 && !isspace(line[0])) {
            current_addr += 4;
        }
    }
}

// ============ 第二遍：生成代码 ============
uint32_t encode_r_type(uint8_t opcode, uint8_t funct3, uint8_t funct7,
                       uint8_t rd, uint8_t rs1, uint8_t rs2) {
    return (funct7 << 25) | (rs2 << 20) | (rs1 << 15) |
           (funct3 << 12) | (rd << 7) | opcode;
}

uint32_t encode_i_type(uint8_t opcode, uint8_t funct3,
                       uint8_t rd, uint8_t rs1, int32_t imm) {
    return ((imm & 0xFFF) << 20) | (rs1 << 15) | (funct3 << 12) |
           (rd << 7) | opcode;
}

// 更多编码函数...

void second_pass(FILE *fp, FILE *out) {
    // 类似first_pass，但生成机器码
    // ...
}

// ============ 主函数 ============
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input.s> [output.bin]\n", argv[0]);
        return 1;
    }

    FILE *in = fopen(argv[1], "r");
    if (!in) {
        perror("Cannot open input");
        return 1;
    }

    // 第一遍
    first_pass(in);
    rewind(in);

    // 打印符号表（调试）
    printf("Symbol Table:\n");
    for (int i = 0; i < sym_count; i++) {
        printf("  %s = 0x%08X\n", symtab[i].name, symtab[i].address);
    }

    // 第二遍
    FILE *out = stdout;
    if (argc > 2) {
        out = fopen(argv[2], "wb");
    }
    second_pass(in, out);

    fclose(in);
    if (out != stdout) fclose(out);

    return 0;
}
```

---

## 7. 学习检查点

### 7.1 自测问题

**基础理解：**

1. RISC-V有多少种基本指令格式？它们的用途分别是什么？
2. 为什么B型和J型的立即数编码如此"奇怪"？（提示：硬件简化）
3. 伪指令`li`在什么情况下会展开为两条指令？

**进阶分析：**
4. 解释为什么汇编器需要两遍扫描？单遍扫描会遇到什么问题？
5. 比较RISC-V汇编器与Hack汇编器的主要区别。
6. 描述符号解析和重定位的过程，为什么链接器需要重定位信息？

**动手实践：**
7. 手工编码以下指令为32位机器码：

- `add x5, x6, x7`
- `addi x8, x9, -100`
- `sw x10, 12(x11)`

1. 编写一个极简汇编器，能够处理5条基本指令。

### 7.2 答案与解析

<details>
<summary>点击查看答案</summary>

**问题1：**
RISC-V有6种基本指令格式：R型（寄存器操作）、I型（立即数/加载）、S型（存储）、B型（分支）、U型（高位立即数）、J型（跳转）。

**问题2：**
B型和J型的立即数位序混乱是为了硬件简化。这种设计使得立即数在指令中的分布与数据通路中的符号扩展单元位置对应，减少了多路选择器的复杂性。

**问题3：**
当立即数超出12位有符号整数范围（-2048到+2047）时，`li`需要展开为`lui`+`addi`两条指令。

**问题4：**
汇编器需要两遍扫描因为：第一遍需要知道所有符号的地址，第二遍才能正确解析前向引用（如分支到后面的标签）。单遍扫描无法处理前向引用。

**问题5：**
主要区别：

- Hack：2种指令格式，无寄存器，16位固定宽度
- RISC-V：6种指令格式，32个寄存器，32位，支持伪指令和重定位

**问题6：**
符号解析建立名称到地址的映射。重定位信息记录哪些位置需要在链接时或加载时修改，以使用最终的运行时地址。

**问题7手工编码：**

```
add x5, x6, x7:
  0000000 00111 00110 000 00101 0110011 = 0x007302B3

addi x8, x9, -100:
  -100 = 0xFF9C (12位: 100111100100)
  100111100100 01001 000 01000 0010011 = 0x9C448413

sw x10, 12(x11):
  imm[11:5]=0000000, imm[4:0]=01100
  0000000 01010 01011 010 01100 0100011 = 0x02A5A623
```

</details>

---

## 8. 与知识库的深层连接

### 8.1 纵向连接（知识链条）

```
理论基础层:
├── [图灵机](../21_Computation_Theory/01_Turing_Machines.md)
│   └── "任何可计算函数都可用图灵机实现"
│
物理实现层:
├── [数字逻辑基础](../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/01_Digital_Logic_Foundations/README.md)
│   └── 逻辑门是实现计算的基础单元
│
├── [RISC-V CPU实现](../02_Formal_Semantics_and_Physics/10_ISA_Machine_Code/08_RISCV_CPU_Complete/README.md)
│   └── 本汇编器生成的代码在此执行
│
当前层（汇编器）:
└── 你在这里 → 汇编器将符号翻译为机器码

上层抽象:
├── [C到汇编：数据表示](./03_C_to_Assembly_Data.md)
│   └── 理解C数据如何映射到本汇编器的符号
│
└── [C到汇编：控制流](./04_C_to_Assembly_Control.md)
    └── 理解C控制结构如何映射到本汇编器的指令
```

### 8.2 横向连接（相关主题）

| 相关文档 | 连接点 |
|:---------|:-------|
| [形式语义基础](../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/01_Operational_Semantics.md) | 汇编器实现了操作语义到机器码的转换 |
| [链接与加载](../02_Formal_Semantics_and_Physics/08_Linking_Loading_Topology/README.md) | 重定位信息是链接器的输入 |
| [编译器基础](../01_Core_Knowledge_System/15_Compiler_Basics/01_Lexical_Analysis.md) | 汇编器是编译器后端的基础组件 |

---

## 9. 进一步阅读

### 9.1 官方文档

- [RISC-V Instruction Set Manual](https://riscv.org/technical/specifications/)
- [RISC-V Assembly Programmer's Manual](https://github.com/riscv/riscv-asm-manual)
- [ELF Specification](https://refspecs.linuxfoundation.org/elf/elf.pdf)

### 9.2 推荐实践

1. **扩展本汇编器**：添加更多伪指令支持
2. **实现链接器**：处理多个目标文件
3. **阅读GNU as源码**：理解工业级汇编器
4. **实现反汇编器**：练习编码的逆操作

---

*文档创建: 2026-03-29*
*上次更新: 2026-03-29*
*状态: 核心框架完成*
