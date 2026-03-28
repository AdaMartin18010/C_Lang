# Hack 和 RISC-V 汇编器

本目录包含两个教学用汇编器实现，配套文档位于 `knowledge/22_Assembler_Implementation/`。

## 文件结构

```
assembler/
├── hack_assembler.c      # Hack汇编器源码
├── riscv_assembler.c     # RISC-V汇编器源码
├── Makefile              # 构建脚本
├── README.md             # 本文件
└── tests/                # 测试用例
    ├── Add.asm           # Hack: 计算1+2+...+100
    ├── Max.asm           # Hack: 求最大值
    ├── Rect.asm          # Hack: 绘制矩形
    ├── Pong.asm          # Hack: 简化版乒乓球游戏
    ├── SymbolTest.asm    # Hack: 符号表测试
    ├── test1.s           # RISC-V: 基础算术
    ├── test2.s           # RISC-V: 加载/存储
    ├── test3.s           # RISC-V: 分支和跳转
    ├── test4.s           # RISC-V: 伪指令
    └── test5.s           # RISC-V: 函数调用
```

## 快速开始

### 编译

```bash
# 编译所有汇编器
make

# 只编译Hack汇编器
make hack

# 只编译RISC-V汇编器
make riscv

# 调试版本（带符号表）
make debug
```

### 运行测试

```bash
# 运行所有测试
make test

# 只运行Hack测试
make test-hack

# 只运行RISC-V测试
make test-riscv
```

### 使用Hack汇编器

```bash
# 基本用法
./hack_assembler input.asm

# 指定输出文件
./hack_assembler input.asm -o output.hack

# 显示详细信息
./hack_assembler input.asm -v

# 显示符号表
./hack_assembler input.asm -s

# 启用调试输出
HACK_DEBUG=1 ./hack_assembler input.asm
```

**输出格式**: `.hack` 文件每行包含16位二进制数，表示一条Hack机器指令。

### 使用RISC-V汇编器

```bash
# 基本用法（二进制输出）
./riscv_assembler input.s

# 文本格式输出（可阅读）
./riscv_assembler input.s -t

# 指定输出文件
./riscv_assembler input.s -o output.bin

# 显示符号表
./riscv_assembler input.s -s

# 组合选项
./riscv_assembler input.s -t -o output.txt -s
```

**输出格式**:
- 二进制模式 (默认): 原始机器码，每指令4字节
- 文本模式 (`-t`): 人类可读的地址+机器码+汇编注释

## Hack汇编器特性

### 支持的指令

| 类型 | 格式 | 示例 |
|------|------|------|
| A指令 | `@value` | `@21`, `@SCREEN`, `@i` |
| C指令 | `dest=comp;jump` | `D=A`, `M=D`, `D;JGT` |
| 标签 | `(LABEL)` | `(LOOP)`, `(END)` |

### 预定义符号

- `R0`-`R15`: 寄存器0-15
- `SP`: 栈指针 (0)
- `LCL`: 局部段 (1)
- `ARG`: 参数段 (2)
- `THIS`: this段 (3)
- `THAT`: that段 (4)
- `SCREEN`: 屏幕地址 (16384)
- `KBD`: 键盘地址 (24576)

### 示例程序

```asm
// Add.asm: 计算1+2+...+100
    @i
    M=1
    @sum
    M=0
(LOOP)
    @i
    D=M
    @100
    D=D-A
    @END
    D;JGT
    @i
    D=M
    @sum
    M=D+M
    @i
    M=M+1
    @LOOP
    0;JMP
(END)
    @END
    0;JMP
```

## RISC-V汇编器特性

### 支持的指令集

- **RV32I**: 32位基础整数指令集
- **RV32M**: 乘除法扩展 (mul, div, rem)

### 指令类型

| 类型 | 用途 | 示例 |
|------|------|------|
| R型 | 寄存器操作 | `add x1, x2, x3` |
| I型 | 立即数/加载 | `addi x1, x2, 100`, `lw x1, 0(x2)` |
| S型 | 存储 | `sw x1, 8(x2)` |
| B型 | 分支 | `beq x1, x2, label` |
| U型 | 高位立即数 | `lui x1, 0x12345` |
| J型 | 跳转 | `jal x1, label` |

### 伪指令

| 伪指令 | 展开 | 说明 |
|--------|------|------|
| `nop` | `addi x0, x0, 0` | 空操作 |
| `mv rd, rs` | `addi rd, rs, 0` | 寄存器移动 |
| `li rd, imm` | `addi` 或 `lui+addi` | 加载立即数 |
| `j label` | `jal x0, label` | 无条件跳转 |
| `jr rs` | `jalr x0, rs, 0` | 寄存器跳转 |
| `ret` | `jalr x0, ra, 0` | 函数返回 |
| `call label` | `auipc+jalr` | 函数调用 |
| `ble rs1, rs2, label` | `bge rs2, rs1, label` | 小于等于分支 |
| `bgt rs1, rs2, label` | `blt rs2, rs1, label` | 大于分支 |

### 寄存器名称

支持x0-x31和ABI名称：
- `zero` (x0): 零寄存器
- `ra` (x1): 返回地址
- `sp` (x2): 栈指针
- `gp` (x3): 全局指针
- `tp` (x4): 线程指针
- `t0-t6` (x5-7, x28-31): 临时寄存器
- `s0-s11` (x8-9, x18-27): 保存寄存器
- `a0-a7` (x10-17): 函数参数/返回值

### 示例程序

```asm
# 计算阶乘
.text
global main
main:
    li   a0, 5
    jal  ra, factorial
end:
    j    end

factorial:
    addi sp, sp, -8
    sw   ra, 4(sp)
    sw   s1, 0(sp)
    
    mv   s1, a0
    addi t0, zero, 1
    ble  s1, t0, base
    
    addi a0, s1, -1
    jal  ra, factorial
    mul  a0, s1, a0
    j    done

base:
    li   a0, 1

done:
    lw   s1, 0(sp)
    lw   ra, 4(sp)
    addi sp, sp, 8
    ret
```

## 实现细节

### Hack汇编器

- **两遍扫描算法**:
  1. 第一遍：收集标签，构建符号表
  2. 第二遍：生成机器码，解析变量
- **符号表**: 简单数组实现，支持预定义符号和运行时变量分配
- **地址分配**: 变量从RAM 16开始自动分配

### RISC-V汇编器

- **两遍扫描算法**:
  1. 第一遍：构建符号表，计算指令地址（考虑伪指令展开）
  2. 第二遍：生成机器码，解析符号引用
- **伪指令展开**: 运行时展开为等效指令序列
- **立即数编码**: 正确处理各种立即数格式的位分布

## 故障排除

### Hack汇编器

**问题**: 符号未定义
- 检查标签格式是否正确 `(LABEL)`
- 确保标签定义在引用之前或使用两遍扫描

**问题**: 非法指令
- C指令必须使用有效的comp/dest/jump组合
- 参考文档中的编码表

### RISC-V汇编器

**问题**: 立即数超出范围
- I型立即数: -2048 到 2047
- 对于大立即数使用 `li` 伪指令

**问题**: 分支目标太远
- B型分支范围: ±4KB
- J型跳转范围: ±1MB
- 远距离跳转使用 `j` 配合寄存器

## 参考资料

- [Nand2Tetris课程](https://www.nand2tetris.org/)
- [RISC-V指令集手册](https://riscv.org/technical/specifications/)
- [RISC-V汇编编程手册](https://github.com/riscv/riscv-asm-manual)

## 配套文档

- `knowledge/22_Assembler_Implementation/01_Hack_Assembler.md`
- `knowledge/22_Assembler_Implementation/02_RISCV_Assembler.md`
