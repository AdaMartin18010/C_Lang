# Hack汇编器实现

> **层级定位**: 22 Assembler Implementation / 基础层
> **前置知识**: [理论到物理桥梁](../21_Computation_Theory/04_Theory_to_Physical_Bridge.md)
> **后续延伸**: [RISC-V汇编器](./02_RISCV_Assembler.md)
> **横向关联**: [Hack计算机架构](../02_Formal_Semantics_and_Physics/10_ISA_Machine_Code/08_RISCV_CPU_Complete/README.md)（参考对比）
> **预计学习时间**: 1周
> **实践验证**: [hack_assembler.c](../../examples/assembler/hack_assembler.c)

---

## 核心洞察

```
汇编器是第一种"编译器"，
它将人类可读的汇编语言翻译为机器可执行的二进制码。

通过实现汇编器，您将理解：
1. 符号解析 - 如何处理标签和变量
2. 指令编码 - 汇编指令如何变成二进制
3. 两遍扫描 - 为什么需要两次遍历
4. 地址分配 - 如何将符号映射到地址

Hack汇编器是最简单的实现，
它来自Nand2Tetris课程，
是理解汇编器原理的最佳起点。
```

---

## 1. Hack计算机概述

### 1.1 架构特点

```
Hack是Nand2Tetris课程设计的16位教学计算机：

- 字长: 16位
- 内存: 32K字（16位地址空间）
  - 0-16383: RAM（数据）
  - 16384-24575: 内存映射I/O（屏幕）
  - 24576: 键盘寄存器
- 指令集: 两种指令类型（A指令和C指令）
- 寄存器: 无显式寄存器，通过A寄存器间接访问

设计哲学: 极简但图灵完备
```

### 1.2 指令格式

```
A指令（地址/立即数）:
  0 v v v v v v v v v v v v v v v
  └┘└────────────┬──────────────┘
  0          15位值

  - 最高位为0表示A指令
  - 低15位为常数值或地址
  - 加载到A寄存器

C指令（计算）:
  1 1 1 a c1c2c3c4c5c6 d1d2 d3 j1j2j3
  └┘└┘└┘└────┬─────┘ └┬┘ └┬┘ └──┬──┘
  固定      计算    目标  跳转
  111

  - 最高位为1表示C指令
  - a: 使用A寄存器还是M（内存[A]）
  - c1-c6: 6位计算控制
  - d1-d3: 3位目标（A, D, M的写入控制）
  - j1-j3: 3位跳转条件
```

### 1.3 C指令详细编码

```
计算字段 (comp):
+------------------+---+--------------------------------+
| 计算             | a | c1 c2 c3 c4 c5 c6              |
+------------------+---+--------------------------------+
| 0                | 0 | 1  0  1  0  1  0               |
| 1                | 0 | 1  1  1  1  1  1               |
| -1               | 0 | 1  1  1  0  1  0               |
| D                | 0 | 0  0  1  1  0  0               |
| A                | 0 | 1  1  0  0  0  0               |
| !D               | 0 | 0  0  1  1  0  1               |
| !A               | 0 | 1  1  0  0  0  1               |
| -D               | 0 | 0  0  1  1  1  1               |
| -A               | 0 | 1  1  0  0  1  1               |
| D+1              | 0 | 0  1  1  1  1  1               |
| A+1              | 0 | 1  1  0  1  1  1               |
| D-1              | 0 | 0  0  1  1  1  0               |
| A-1              | 0 | 1  1  0  0  1  0               |
| D+A              | 0 | 0  0  0  0  1  0               |
| D-A              | 0 | 0  1  0  0  1  1               |
| A-D              | 0 | 0  0  0  1  1  1               |
| D&A              | 0 | 0  0  0  0  0  0               |
| D|A              | 0 | 0  1  0  1  0  1               |
+------------------+---+--------------------------------+
| M（将A替换为M）   | 1 | （同上，a=1）                   |
+------------------+---+--------------------------------+

目标字段 (dest):
+-------+--------+--------+
| d1d2d3 | 目标   | 含义   |
+-------+--------+--------+
| 0 0 0 | null   | 不存储 |
| 0 0 1 | M      | 内存[A]|
| 0 1 0 | D      | D寄存器|
| 0 1 1 | MD     | M和D   |
| 1 0 0 | A      | A寄存器|
| 1 0 1 | AM     | A和M   |
| 1 1 0 | AD     | A和D   |
| 1 1 1 | AMD    | 全部   |
+-------+--------+--------+

跳转字段 (jump):
+-------+--------+-------------+
| j1j2j3 | 跳转   | 条件        |
+-------+--------+-------------+
| 0 0 0 | null   | 不跳转      |
| 0 0 1 | JGT    | 如果>0      |
| 0 1 0 | JEQ    | 如果=0      |
| 0 1 1 | JGE    | 如果≥0      |
| 1 0 0 | JLT    | 如果<0      |
| 1 0 1 | JNE    | 如果≠0      |
| 1 1 0 | JLE    | 如果≤0      |
| 1 1 1 | JMP    | 无条件跳转  |
+-------+--------+-------------+
```

---

## 2. Hack汇编语言

### 2.1 指令语法

```asm
; A指令 - 地址/立即数
@value      ; 将value加载到A寄存器

; value可以是：
@21         ; 十进制立即数
@SYMBOL     ; 符号（变量或标签）
@R0         ; 预定义符号（R0-R15对应RAM 0-15）
@SCREEN     ; 预定义符号（16384）
@KBD        ; 预定义符号（24576）

; C指令 - 计算
dest=comp;jump   ; 完整形式
comp;jump        ; 无dest
dest=comp        ; 无jump
comp             ; 无dest无jump

; 示例：
D=A             ; D = A
D=D+A           ; D = D + A
M=D             ; RAM[A] = D
D=M             ; D = RAM[A]
D=D-M           ; D = D - RAM[A]
0;JMP           ; 无条件跳转到A指定的地址
D;JGT           ; 如果D>0，跳转到A指定的地址
```

### 2.2 符号处理

```asm
; 1. 预定义符号
@R0     ; = 0
@R1     ; = 1
...@R15 ; = 15
@SP     ; = 0（栈指针）
@LCL    ; = 1（局部段基址）
@ARG    ; = 2（参数段基址）
@THIS   ; = 3（this段基址）
@THAT   ; = 4（that段基址）
@SCREEN ; = 16384
@KBD    ; = 24576

; 2. 标签符号（Label Symbols）
(LOOP)      ; 定义标签，值为下条指令的地址
@LOOP       ; 引用标签，值为(LOOP)的地址
0;JMP       ; 跳转到标签

; 3. 变量符号（Variable Symbols）
@i          ; 首次使用，分配到RAM 16
@sum        ; 首次使用，分配到RAM 17
@i          ; 再次使用，引用RAM 16
```

### 2.3 完整示例程序

```asm
; Add.asm: 计算1+2+...+100

    @i      ; i = 1
    M=1

    @sum    ; sum = 0
    M=0

(LOOP)      ; 循环开始
    @i
    D=M     ; D = i

    @100
    D=D-A   ; D = i - 100

    @END
    D;JGT   ; 如果i>100，跳转到END

    @i
    D=M     ; D = i

    @sum
    M=D+M   ; sum = sum + i

    @i
    M=M+1   ; i = i + 1

    @LOOP
    0;JMP   ; 跳转到LOOP

(END)
    @END
    0;JMP   ; 无限循环（停机）
```

---

## 3. 汇编器实现

### 3.1 设计概述

```
两遍扫描算法：

第一遍（符号解析）：
  - 扫描所有标签定义（Xxx）
  - 记录标签对应的指令地址
  - 维护指令地址计数器（不考虑A指令中的常数）

第二遍（代码生成）：
  - 翻译每条指令为二进制
  - 遇到符号时查询符号表
  - 新变量分配到RAM 16开始的位置
  - 输出.hack文件

数据结构：
  - 符号表: HashMap<符号名, 地址>
  - 预定义符号初始化
  - 变量分配计数器（从16开始）
```

### 3.2 C语言实现

```c
// examples/assembler/hack_assembler.c
// Hack汇编器完整实现

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define MAX_LINE 256
#define MAX_SYMBOLS 1024
#define MAX_PROGRAM 32768

// 符号表条目
typedef struct {
    char name[MAX_LINE];
    int address;
} SymbolEntry;

// 符号表
typedef struct {
    SymbolEntry entries[MAX_SYMBOLS];
    int count;
} SymbolTable;

// 初始化预定义符号
void init_symbol_table(SymbolTable *table) {
    table->count = 0;

    // 预定义寄存器 R0-R15
    for (int i = 0; i <= 15; i++) {
        char name[8];
        sprintf(name, "R%d", i);
        strcpy(table->entries[table->count].name, name);
        table->entries[table->count].address = i;
        table->count++;
    }

    // 其他预定义符号
    const char *predefined[][2] = {
        {"SP", "0"}, {"LCL", "1"}, {"ARG", "2"},
        {"THIS", "3"}, {"THAT", "4"},
        {"SCREEN", "16384"}, {"KBD", "24576"},
        {NULL, NULL}
    };

    for (int i = 0; predefined[i][0] != NULL; i++) {
        strcpy(table->entries[table->count].name, predefined[i][0]);
        table->entries[table->count].address = atoi(predefined[i][1]);
        table->count++;
    }
}

// 查找符号
int lookup_symbol(SymbolTable *table, const char *name, int *address) {
    for (int i = 0; i < table->count; i++) {
        if (strcmp(table->entries[i].name, name) == 0) {
            *address = table->entries[i].address;
            return 1;  // 找到
        }
    }
    return 0;  // 未找到
}

// 添加符号
void add_symbol(SymbolTable *table, const char *name, int address) {
    if (table->count >= MAX_SYMBOLS) {
        fprintf(stderr, "Error: Symbol table full\n");
        exit(1);
    }
    strcpy(table->entries[table->count].name, name);
    table->entries[table->count].address = address;
    table->count++;
}

// 清理行（移除注释和空白）
void clean_line(char *line) {
    // 移除//注释
    char *comment = strstr(line, "//");
    if (comment) *comment = '\0';

    // 移除行尾空白
    int len = strlen(line);
    while (len > 0 && isspace((unsigned char)line[len-1])) {
        line[--len] = '\0';
    }

    // 移除行首空白
    char *start = line;
    while (isspace((unsigned char)*start)) start++;
    if (start != line) memmove(line, start, strlen(start) + 1);
}

// 解析A指令
int parse_a_instruction(const char *line, char *symbol) {
    if (line[0] != '@') return 0;
    strcpy(symbol, line + 1);
    return 1;
}

// 解析标签 (Xxx)
int parse_label(const char *line, char *label) {
    if (line[0] != '(') return 0;

    const char *end = strchr(line, ')');
    if (!end) return 0;

    int len = end - line - 1;
    strncpy(label, line + 1, len);
    label[len] = '\0';
    return 1;
}

// 解析C指令
int parse_c_instruction(const char *line, char *dest, char *comp, char *jump) {
    char temp[MAX_LINE];
    strcpy(temp, line);

    *dest = '\0';
    *comp = '\0';
    *jump = '\0';

    // 查找等号（dest=comp）
    char *equal = strchr(temp, '=');
    char *semi = strchr(temp, ';');

    if (equal) {
        *equal = '\0';
        strcpy(dest, temp);

        if (semi) {
            *semi = '\0';
            strcpy(comp, equal + 1);
            strcpy(jump, semi + 1);
        } else {
            strcpy(comp, equal + 1);
        }
    } else if (semi) {
        *semi = '\0';
        strcpy(comp, temp);
        strcpy(jump, semi + 1);
    } else {
        strcpy(comp, temp);
    }

    return 1;
}

// comp编码表
const char *comp_table[] = {
    "0",   "101010",
    "1",   "111111",
    "-1",  "111010",
    "D",   "001100",
    "A",   "110000",
    "M",   "110000",  // a=1
    "!D",  "001101",
    "!A",  "110001",
    "!M",  "110001",  // a=1
    "-D",  "001111",
    "-A",  "110011",
    "-M",  "110011",  // a=1
    "D+1", "011111",
    "A+1", "110111",
    "M+1", "110111",  // a=1
    "D-1", "001110",
    "A-1", "110010",
    "M-1", "110010",  // a=1
    "D+A", "000010",
    "D+M", "000010",  // a=1
    "D-A", "010011",
    "D-M", "010011",  // a=1
    "A-D", "000111",
    "M-D", "000111",  // a=1
    "D&A", "000000",
    "D&M", "000000",  // a=1
    "D|A", "010101",
    "D|M", "010101",  // a=1
    NULL, NULL
};

// 编码comp
int encode_comp(const char *comp, char *binary) {
    int a_bit = 0;
    const char *comp_lookup = comp;

    // 检查是否使用M（内存）
    if (strchr(comp, 'M')) {
        a_bit = 1;
        // 移除a位后的编码与A版本相同
    }

    for (int i = 0; comp_table[i] != NULL; i += 2) {
        if (strcmp(comp_table[i], comp_lookup) == 0) {
            sprintf(binary, "%d%s", a_bit, comp_table[i+1]);
            return 1;
        }
    }
    return 0;
}

// 编码dest
int encode_dest(const char *dest, char *binary) {
    int d1 = strchr(dest, 'A') ? 1 : 0;
    int d2 = strchr(dest, 'D') ? 1 : 0;
    int d3 = strchr(dest, 'M') ? 1 : 0;
    sprintf(binary, "%d%d%d", d1, d2, d3);
    return 1;
}

// 编码jump
int encode_jump(const char *jump, char *binary) {
    if (strlen(jump) == 0) {
        strcpy(binary, "000");
    } else if (strcmp(jump, "JGT") == 0) strcpy(binary, "001");
    else if (strcmp(jump, "JEQ") == 0) strcpy(binary, "010");
    else if (strcmp(jump, "JGE") == 0) strcpy(binary, "011");
    else if (strcmp(jump, "JLT") == 0) strcpy(binary, "100");
    else if (strcmp(jump, "JNE") == 0) strcpy(binary, "101");
    else if (strcmp(jump, "JLE") == 0) strcpy(binary, "110");
    else if (strcmp(jump, "JMP") == 0) strcpy(binary, "111");
    else return 0;
    return 1;
}

// 第一遍：构建符号表
void first_pass(FILE *input, SymbolTable *table, int *line_count) {
    char line[MAX_LINE];
    char symbol[MAX_LINE];
    int rom_address = 0;

    while (fgets(line, sizeof(line), input)) {
        clean_line(line);
        if (strlen(line) == 0) continue;

        if (parse_label(line, symbol)) {
            // 标签定义，记录地址
            add_symbol(table, symbol, rom_address);
        } else {
            // 普通指令，地址+1
            rom_address++;
        }
    }

    *line_count = rom_address;
    rewind(input);
}

// 第二遍：生成代码
void second_pass(FILE *input, FILE *output, SymbolTable *table) {
    char line[MAX_LINE];
    char symbol[MAX_LINE];
    char dest[MAX_LINE], comp[MAX_LINE], jump[MAX_LINE];
    int next_ram = 16;  // 变量从16开始分配

    while (fgets(line, sizeof(line), input)) {
        clean_line(line);
        if (strlen(line) == 0) continue;

        // 跳过标签定义
        if (parse_label(line, symbol)) continue;

        uint16_t instruction = 0;

        // A指令
        if (parse_a_instruction(line, symbol)) {
            int value;
            if (isdigit((unsigned char)symbol[0])) {
                // 常数
                value = atoi(symbol);
            } else {
                // 符号
                if (!lookup_symbol(table, symbol, &value)) {
                    // 新变量
                    value = next_ram++;
                    add_symbol(table, symbol, value);
                }
            }
            instruction = (uint16_t)value;
            fprintf(output, "%016d\n",
                (int)(instruction & 0x7FFF));  // A指令最高位为0
        }
        // C指令
        else if (parse_c_instruction(line, dest, comp, jump)) {
            char comp_bits[8], dest_bits[4], jump_bits[4];

            if (!encode_comp(comp, comp_bits)) {
                fprintf(stderr, "Error: Unknown comp: %s\n", comp);
                continue;
            }
            encode_dest(dest, dest_bits);
            encode_jump(jump, jump_bits);

            // 111 + a + comp + dest + jump
            fprintf(output, "111%s%s%s\n", comp_bits, dest_bits, jump_bits);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input.asm>\n", argv[0]);
        return 1;
    }

    char *input_name = argv[1];
    char output_name[MAX_LINE];

    // 生成输出文件名
    strcpy(output_name, input_name);
    char *dot = strrchr(output_name, '.');
    if (dot) strcpy(dot, ".hack");
    else strcat(output_name, ".hack");

    FILE *input = fopen(input_name, "r");
    if (!input) {
        perror("Error opening input file");
        return 1;
    }

    FILE *output = fopen(output_name, "w");
    if (!output) {
        perror("Error opening output file");
        fclose(input);
        return 1;
    }

    // 初始化符号表
    SymbolTable table;
    init_symbol_table(&table);

    // 第一遍
    int line_count;
    first_pass(input, &table, &line_count);
    printf("First pass: %d instructions, %d symbols\n",
           line_count, table.count);

    // 第二遍
    second_pass(input, output, &table);
    printf("Assembly complete: %s\n", output_name);

    fclose(input);
    fclose(output);

    return 0;
}
```

---

## 4. 测试验证

### 4.1 测试用例

```asm
; test.asm - 测试各种指令类型

// A指令测试
@0          // 常数0
@21         // 常数21
@R0         // 预定义符号
@SCREEN     // 预定义符号
@i          // 变量（应分配到16）
@sum        // 变量（应分配到17）

// C指令测试
D=A
D=D+A
M=D
D=M
D=D-M
0;JMP
D;JGT

// 标签测试
(LOOP)
@LOOP
0;JMP
```

### 4.2 预期输出

```
0000000000000000    // @0
0000000000010101    // @21
0000000000000000    // @R0
0100000000000000    // @SCREEN (16384)
0000000000010000    // @i (16)
0000000000010001    // @sum (17)
1110110000010000    // D=A
1110000010010000    // D=D+A
1110001100001000    // M=D
1111110000010000    // D=M
1111010011010000    // D=D-M
1110101010000111    // 0;JMP
1110001100000001    // D;JGT
0000000000001100    // @LOOP (地址根据实际位置)
1110101010000111    // 0;JMP
```

---

## 5. 与RISC-V的对比

### 5.1 关键差异

```
特性          Hack                RISC-V
─────────────────────────────────────────────────
指令长度      固定16位            可变32/64位
寄存器        A, D, M（隐式）      32个通用寄存器
内存访问      通过A寄存器间接      基址+偏移寻址
指令类型      只有A和C两种         多种（R/I/S/B/U/J）
复杂性        极简（教学用）        实用（工业级）

共同点：
- 都是加载-存储架构
- 都使用汇编器将文本翻译为二进制
- 都使用符号表处理标签和变量
```

### 5.2 汇编器的通用结构

```
无论目标架构如何，汇编器的核心结构相同：

1. 词法分析（Lexer）
   - 将输入分解为token
   - 识别指令、寄存器、立即数、符号

2. 语法分析（Parser）
   - 识别指令类型
   - 提取操作数

3. 符号解析（Symbol Resolution）
   - 第一遍：收集标签定义
   - 第二遍：解析符号引用

4. 代码生成（Code Generation）
   - 将汇编指令编码为二进制
   - 输出目标文件

5. 错误处理
   - 语法错误
   - 未定义符号
   - 数值溢出
```

---

## 6. 学习检查点

完成本章后，您应该能够：

- [ ] 理解Hack指令格式（A指令和C指令）
- [ ] 手工将Hack汇编翻译为二进制
- [ ] 解释两遍扫描汇编器的工作原理
- [ ] 运行并测试hack_assembler.c
- [ ] 对比Hack与RISC-V的差异

---

## 7. 下一步

### 下一章：[RISC-V汇编器](./02_RISCV_Assembler.md)

我们将实现一个更实用的RISC-V汇编器，支持：

- RISC-V标准指令集
- 伪指令展开
- 重定位概念
- 与您的RISC-V CPU实现连接

---

*文档创建: 2026-03-29*
*配套代码: [hack_assembler.c](../../examples/assembler/hack_assembler.c)*
*状态: Month 2 文档1/4*
