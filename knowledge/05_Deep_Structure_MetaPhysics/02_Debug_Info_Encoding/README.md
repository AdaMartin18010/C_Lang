# 调试信息编码

## 概述

本文档深入探讨调试信息编码的底层机制，主要关注 DWARF（Debugging With Attributed Record Formats）格式——现代编译器（GCC、Clang、MSVC）和调试器（GDB、LLDB）广泛采用的标准。理解调试信息的编码方式对于开发调试工具、分析崩溃转储以及优化二进制文件大小至关重要。

## 目录

1. [DWARF 格式基础](#dwarf-格式基础)
2. [调试符号表](#调试符号表)
3. [行号信息编码](#行号信息编码)
4. [变量位置追踪](#变量位置追踪)
5. [高级主题](#高级主题)

---

## DWARF 格式基础

### DWARF 版本演进

DWARF 标准经历了多个版本的演进，每个版本都增加了新的特性以支持现代编程语言构造：

| 版本 | 发布年份 | 主要特性 |
|------|----------|----------|
| DWARF 2 | 1993 | 基础结构，.debug_info 节 |
| DWARF 3 | 2005 | 改进的 C++ 支持，压缩行表 |
| DWARF 4 | 2010 | 类型单元，宏信息，改进的优化代码支持 |
| DWARF 5 | 2017 | 分离式 DWARF，改进的索引，更好的压缩 |

### DWARF 节结构

```c
// DWARF 核心节（存储在 ELF 文件中）
// 每个节都有特定的用途和数据格式

#define DWARF_SECTIONS \
    X(debug_abbrev, "缩略语表 - 定义 DIE 的属性编码") \
    X(debug_info,   "主调试信息 - 编译单元和 DIE 树") \
    X(debug_line,   "行号程序 - 代码到源文件的映射") \
    X(debug_str,    "字符串表 - 共享字符串数据") \
    X(debug_loc,    "位置列表 - 变量位置随代码变化") \
    X(debug_ranges, "范围列表 - DIE 的地址范围") \
    X(debug_frame,  "调用帧信息 - 栈展开数据") \
    X(debug_aranges, "地址范围表 - 快速地址查找") \
    X(debug_pubnames, "公共名称 - 快速符号查找") \
    X(debug_types,  "类型单元 - DWARF 4+ 的类型定义")
```

### DIE（调试信息条目）

DIE 是 DWARF 的基本构建块，形成树状结构描述程序中的各种实体：

```c
// DIE 的内存布局
struct Dwarf_Die {
    uint16_t tag;           // 标签，如 DW_TAG_subprogram
    uint8_t has_children;   // 是否有子 DIE
    
    // 属性列表（变长）
    struct {
        uint16_t attr;      // 属性名，如 DW_AT_name
        uint8_t form;       // 值编码形式
        union {
            uint64_t udata; // 无符号整数
            int64_t sdata;  // 有符号整数
            uint32_t str_idx; // 字符串表索引
            uint64_t ref;   // 对其他 DIE 的引用
            uint64_t addr;  // 地址
            // 其他形式...
        } value;
    } attributes[];
};

// 常见 DIE 标签
enum Dwarf_Tag {
    DW_TAG_compile_unit = 0x11,    // 编译单元
    DW_TAG_subprogram = 0x2e,      // 函数/子程序
    DW_TAG_variable = 0x34,        // 变量
    DW_TAG_formal_parameter = 0x5, // 形式参数
    DW_TAG_base_type = 0x24,       // 基本类型
    DW_TAG_pointer_type = 0x0f,    // 指针类型
    DW_TAG_structure_type = 0x13,  // 结构体类型
    DW_TAG_member = 0x0d,          // 结构体成员
    DW_TAG_typedef = 0x16,         // 类型定义
    DW_TAG_array_type = 0x01,      // 数组类型
    DW_TAG_subrange_type = 0x21,   // 数组下标范围
    DW_TAG_enumeration_type = 0x04,// 枚举类型
    DW_TAG_enumerator = 0x28,      // 枚举常量
    DW_TAG_lexical_block = 0x0b,   // 词法块
    DW_TAG_inlined_subroutine = 0x1d, // 内联函数
};
```

### 使用 libdw 读取 DWARF

```c
#include <elfutils/libdw.h>
#include <elfutils/libdwfl.h>
#include <stdio.h>
#include <stdlib.h>

// 遍历编译单元的 DIE 树
void print_die_tree(Dwarf_Die *die, int indent) {
    const char *name = dwarf_diename(die) ?: "<anonymous>";
    const char *tag_name = dwarf_tag_string(dwarf_tag(die));
    
    printf("%*s%s: %s\n", indent * 2, "", tag_name, name);
    
    // 遍历属性
    Dwarf_Attribute attr;
    Dwarf_Off offset = 0;
    while (dwarf_next_attrib(die, &offset, &attr) == 0) {
        unsigned int attr_num = dwarf_whatattr(&attr);
        unsigned int form = dwarf_whatform(&attr);
        printf("%*s  [%s: %s]\n", indent * 2, "",
               dwarf_attr_string(attr_num),
               dwarf_form_string(form));
    }
    
    // 递归遍历子 DIE
    Dwarf_Die child;
    if (dwarf_child(die, &child) == 0) {
        do {
            print_die_tree(&child, indent + 1);
        } while (dwarf_siblingof(&child, &child) == 0);
    }
}

// 主函数：分析 ELF 文件的 DWARF 信息
int analyze_dwarf(const char *filename) {
    Dwfl *dwfl = dwfl_begin(&offline_callbacks);
    if (!dwfl) {
        fprintf(stderr, "Failed to initialize dwfl\n");
        return 1;
    }
    
    Dwarf *dwarf = dwfl_core_file_attach(dwfl, filename);
    if (!dwarf) {
        fprintf(stderr, "Failed to open %s\n", filename);
        dwfl_end(dwfl);
        return 1;
    }
    
    // 遍历所有编译单元
    Dwarf_Off offset = 0;
    Dwarf_Off next_offset;
    while (dwarf_nextcu(dwarf, offset, &next_offset, NULL, NULL, NULL, NULL) == 0) {
        Dwarf_Die cu_die;
        if (dwarf_offdie(dwarf, offset, &cu_die) != NULL) {
            printf("=== Compilation Unit ===\n");
            print_die_tree(&cu_die, 0);
        }
        offset = next_offset;
    }
    
    dwfl_end(dwfl);
    return 0;
}
```

---

## 调试符号表

### 符号表结构

```c
// ELF 符号表条目（.symtab 和 .dynsym）
struct Elf64_Sym {
    uint32_t st_name;   // 字符串表索引
    uint8_t  st_info;   // 类型和绑定属性
    uint8_t  st_other;  // 可见性
    uint16_t st_shndx;  // 相关节的索引
    uint64_t st_value;  // 符号值（地址）
    uint64_t st_size;   // 符号大小
};

// 符号类型 (ELF32_ST_TYPE)
#define STT_NOTYPE  0   // 未指定类型
#define STT_OBJECT  1   // 数据对象（变量）
#define STT_FUNC    2   // 函数
#define STT_SECTION 3   // 节
#define STT_FILE    4   // 文件名
#define STT_COMMON  5   // 公共数据
#define STT_TLS     6   // 线程本地存储

// 符号绑定 (ELF32_ST_BIND)
#define STB_LOCAL   0   // 局部符号
#define STB_GLOBAL  1   // 全局符号
#define STB_WEAK    2   // 弱符号
```

### 符号解析算法

```c
#include <elf.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

typedef struct {
    const char *name;
    uint64_t address;
    uint64_t size;
    int type;
    int bind;
} SymbolInfo;

// 解析 ELF 符号表
int parse_symbol_table(const char *filename, SymbolInfo **symbols, size_t *count) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) return -1;
    
    struct stat st;
    if (fstat(fd, &st) < 0) {
        close(fd);
        return -1;
    }
    
    void *map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (map == MAP_FAILED) return -1;
    
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)map;
    Elf64_Shdr *sections = (Elf64_Shdr *)((char *)map + ehdr->e_shoff);
    
    // 查找符号表和字符串表
    Elf64_Shdr *symtab = NULL, *strtab = NULL;
    const char *shstrtab = (char *)map + sections[ehdr->e_shstrndx].sh_offset;
    
    for (int i = 0; i < ehdr->e_shnum; i++) {
        const char *name = shstrtab + sections[i].sh_name;
        if (strcmp(name, ".symtab") == 0) symtab = &sections[i];
        if (strcmp(name, ".strtab") == 0) strtab = &sections[i];
    }
    
    if (!symtab || !strtab) {
        munmap(map, st.st_size);
        return -1;
    }
    
    // 解析符号
    Elf64_Sym *syms = (Elf64_Sym *)((char *)map + symtab->sh_offset);
    const char *strings = (char *)map + strtab->sh_offset;
    size_t num_syms = symtab->sh_size / sizeof(Elf64_Sym);
    
    *symbols = malloc(num_syms * sizeof(SymbolInfo));
    *count = 0;
    
    for (size_t i = 0; i < num_syms; i++) {
        if (syms[i].st_name == 0) continue;  // 跳过无名符号
        
        (*symbols)[*count] = (SymbolInfo){
            .name = strings + syms[i].st_name,
            .address = syms[i].st_value,
            .size = syms[i].st_size,
            .type = ELF64_ST_TYPE(syms[i].st_info),
            .bind = ELF64_ST_BIND(syms[i].st_info)
        };
        (*count)++;
    }
    
    munmap(map, st.st_size);
    return 0;
}

// 地址到符号的映射（用于堆栈回溯）
const char *addr_to_symbol(SymbolInfo *symbols, size_t count, uint64_t addr) {
    for (size_t i = 0; i < count; i++) {
        if (addr >= symbols[i].address && 
            addr < symbols[i].address + symbols[i].size) {
            return symbols[i].name;
        }
    }
    return NULL;
}
```

---

## 行号信息编码

### 行号程序状态机

DWARF 使用紧凑的状态机编码行号信息，通过一系列操作码将程序计数器映射到源代码位置：

```c
// 行号程序寄存器（状态机状态）
struct LineNumberState {
    uint64_t address;        // 当前程序地址
    uint64_t op_index;       // 操作索引（用于 VLIW 架构）
    uint32_t file;           // 文件表索引
    uint32_t line;           // 源代码行号
    uint32_t column;         // 源代码列号
    uint8_t  is_stmt;        // 是否是语句开始
    uint8_t  basic_block;    // 是否是基本块开始
    uint8_t  end_sequence;   // 是否结束序列
    uint8_t  prologue_end;   // 函数序言结束
    uint8_t  epilogue_begin; // 函数尾声开始
    uint64_t isa;            // 指令集架构
    uint64_t discriminator;  // 区分同一行的多个代码位置
};

// 标准操作码
enum DwarfLineNumberOpcode {
    DW_LNS_copy             = 1,  // 复制当前状态到行表
    DW_LNS_advance_pc       = 2,  // 按操作数增加地址
    DW_LNS_advance_line     = 3,  // 按有符号操作数增加行号
    DW_LNS_set_file         = 4,  // 设置当前文件
    DW_LNS_set_column       = 5,  // 设置当前列
    DW_LNS_negate_stmt      = 6,  // 切换 is_stmt
    DW_LNS_set_basic_block  = 7,  // 设置 basic_block 标志
    DW_LNS_const_add_pc     = 8,  // 增加地址（操作数隐含）
    DW_LNS_fixed_advance_pc = 9,  // 增加绝对地址
    DW_LNS_set_prologue_end = 10, // 设置序言结束
    DW_LNS_set_epilogue_begin = 11, // 设置尾声开始
    DW_LNS_set_isa          = 12, // 设置 ISA
};

// 特殊操作码编码（压缩）
// opcode = (line_advance - line_base) + (line_range * address_advance) + opcode_base
// 可同时更新地址和行号
```

### 行号程序解析

```c
// 简化的行号程序解析器
int parse_line_number_program(const uint8_t *program, size_t len,
                              LineNumberState *initial_state,
                              void (*callback)(const LineNumberState *)) {
    LineNumberState state = *initial_state;
    const uint8_t *p = program;
    
    while (p < program + len) {
        uint8_t opcode = *p++;
        
        if (opcode == 0) {
            // 扩展操作码
            uint64_t length = decode_uleb128(&p);
            const uint8_t *end = p + length;
            uint8_t ext_opcode = *p++;
            
            switch (ext_opcode) {
                case DW_LNE_end_sequence:
                    state.end_sequence = 1;
                    callback(&state);
                    // 重置状态
                    state = *initial_state;
                    break;
                case DW_LNE_set_address:
                    state.address = read_target_addr(&p);
                    break;
                case DW_LNE_define_file:
                    // 添加文件到文件表
                    break;
            }
            p = end;
        }
        else if (opcode < opcode_base) {
            // 标准操作码
            switch (opcode) {
                case DW_LNS_copy:
                    callback(&state);
                    state.basic_block = 0;
                    state.prologue_end = 0;
                    state.epilogue_begin = 0;
                    state.discriminator = 0;
                    break;
                case DW_LNS_advance_pc:
                    state.address += decode_uleb128(&p) * minimum_instruction_length;
                    break;
                case DW_LNS_advance_line:
                    state.line += decode_sleb128(&p);
                    break;
                case DW_LNS_set_file:
                    state.file = decode_uleb128(&p);
                    break;
                case DW_LNS_set_column:
                    state.column = decode_uleb128(&p);
                    break;
                case DW_LNS_negate_stmt:
                    state.is_stmt = !state.is_stmt;
                    break;
                case DW_LNS_const_add_pc:
                    state.address += ((255 - opcode_base) / line_range) 
                                     * minimum_instruction_length;
                    break;
            }
        }
        else {
            // 特殊操作码
            int adjusted = opcode - opcode_base;
            int line_adv = line_base + (adjusted % line_range);
            int addr_adv = adjusted / line_range;
            
            state.line += line_adv;
            state.address += addr_adv * minimum_instruction_length;
            callback(&state);
            
            state.basic_block = 0;
            state.prologue_end = 0;
            state.epilogue_begin = 0;
            state.discriminator = 0;
        }
    }
    return 0;
}
```

---

## 变量位置追踪

### 位置表达式

DWARF 使用复杂的表达式描述变量在不同执行点的存储位置：

```c
// DWARF 表达式操作码
enum DwarfExpressionOpcode {
    // 堆栈操作
    DW_OP_dup      = 0x12,  // 复制栈顶
    DW_OP_drop     = 0x13,  // 弹出栈顶
    DW_OP_pick     = 0x15,  // 选取栈中元素
    DW_OP_swap     = 0x16,  // 交换栈顶两个元素
    DW_OP_rot      = 0x17,  // 旋转栈顶三个元素
    
    // 算术运算
    DW_OP_abs      = 0x19,
    DW_OP_and      = 0x1a,
    DW_OP_div      = 0x1b,
    DW_OP_minus    = 0x1c,
    DW_OP_mod      = 0x1d,
    DW_OP_mul      = 0x1e,
    DW_OP_neg      = 0x1f,
    DW_OP_not      = 0x20,
    DW_OP_or       = 0x21,
    DW_OP_plus     = 0x22,
    DW_OP_shl      = 0x24,
    DW_OP_shr      = 0x25,
    DW_OP_shra     = 0x26,
    DW_OP_xor      = 0x27,
    
    // 内存访问
    DW_OP_deref    = 0x06,  // 解引用
    DW_OP_deref_size = 0x94,// 带大小限制的解引用
    DW_OP_xderef   = 0x18,  // 跨地址空间解引用
    
    // 寄存器访问
    DW_OP_reg0     = 0x50,  // 寄存器 0-31 的直接编码
    // ... DW_OP_reg31 = 0x6f
    DW_OP_regx     = 0x90,  // 任意寄存器编号
    DW_OP_breg0    = 0x70,  // 基址寄存器 + SLEB128 偏移
    // ... DW_OP_breg31 = 0x8f
    DW_OP_bregx    = 0x92,  // 任意基址寄存器 + 偏移
    
    // 字面量
    DW_OP_lit0     = 0x30,  // 压入无符号字面量 0-31
    // ... DW_OP_lit31 = 0x4f
    DW_OP_addr     = 0x03,  // 压入绝对地址
    DW_OP_const1u  = 0x08,  // 压入 1 字节无符号常量
    DW_OP_const1s  = 0x09,  // 压入 1 字节有符号常量
    DW_OP_const2u  = 0x0a,
    DW_OP_const2s  = 0x0b,
    DW_OP_const4u  = 0x0c,
    DW_OP_const4s  = 0x0d,
    DW_OP_const8u  = 0x0e,
    DW_OP_const8s  = 0x0f,
    DW_OP_constu   = 0x10,  // ULEB128 常量
    DW_OP_consts   = 0x11,  // SLEB128 常量
    
    // 特殊
    DW_OP_fbreg    = 0x91,  // 基于帧寄存器的偏移
    DW_OP_piece    = 0x93,  // 值的一部分（分散在多个位置）
    DW_OP_bit_piece = 0x9d, // 位的子集
    DW_OP_implicit_value = 0x9e, // 隐式值（常量）
    DW_OP_stack_value = 0x9f,    // 值在栈上（不在内存）
};
```

### 位置列表

当变量的位置在函数执行期间发生变化时，使用位置列表描述：

```c
// 位置列表条目
struct LocationListEntry {
    uint64_t start_addr;   // 起始地址（或 base address selection）
    uint64_t end_addr;     // 结束地址
    uint8_t *expression;   // DWARF 表达式
    size_t expr_len;
};

// 解析变量位置
int evaluate_location_expression(const uint8_t *expr, size_t len,
                                  Dwarf_RegState *regs,
                                  uint64_t *result) {
    uint64_t stack[64];
    int sp = 0;
    const uint8_t *p = expr;
    
    while (p < expr + len) {
        uint8_t op = *p++;
        
        switch (op) {
            case DW_OP_reg0 ... DW_OP_reg31:
                stack[sp++] = regs->r[op - DW_OP_reg0];
                break;
                
            case DW_OP_breg0 ... DW_OP_breg31: {
                int64_t offset = decode_sleb128(&p);
                stack[sp++] = regs->r[op - DW_OP_breg0] + offset;
                break;
            }
            
            case DW_OP_addr:
                stack[sp++] = read_target_addr(&p);
                break;
                
            case DW_OP_constu:
                stack[sp++] = decode_uleb128(&p);
                break;
                
            case DW_OP_consts:
                stack[sp++] = decode_sleb128(&p);
                break;
                
            case DW_OP_plus:
                sp--;
                stack[sp-1] += stack[sp];
                break;
                
            case DW_OP_minus:
                sp--;
                stack[sp-1] -= stack[sp];
                break;
                
            case DW_OP_deref:
                // 从计算出的地址读取值
                // 需要目标进程的内存访问
                stack[sp-1] = read_memory(stack[sp-1]);
                break;
                
            case DW_OP_fbreg: {
                int64_t offset = decode_sleb128(&p);
                // 帧基址寄存器 + 偏移
                stack[sp++] = regs->frame_base + offset;
                break;
            }
                
            case DW_OP_stack_value:
                // 值本身就在栈上，不在内存中
                // 常用于寄存器变量
                break;
        }
    }
    
    *result = stack[0];
    return 0;
}
```

### 调用帧信息（CFA）

```c
// .debug_frame 和 .eh_frame 格式
// 用于栈展开（unwinding）

struct CIE {
    uint64_t length;
    uint32_t cie_id;           // CIE ID（0 或 0xffffffff）
    uint8_t version;
    char augmentation[16];      // 扩展字符串
    uint64_t code_alignment_factor;
    int64_t data_alignment_factor;
    uint64_t return_address_register;
    uint8_t initial_instructions[]; // 初始 CFA 程序
};

struct FDE {
    uint64_t length;
    uint32_t cie_pointer;      // 指向相关 CIE 的偏移
    uint64_t initial_location; // 函数起始地址
    uint64_t address_range;    // 函数字节大小
    uint8_t instructions[];    // FDE 特定的 CFA 程序
};

// CFA 指令
enum CFA_Instruction {
    DW_CFA_advance_loc    = 0x40,  //  delta: 增加位置
    DW_CFA_offset         = 0x80,  //  register, ULEB128 offset
    DW_CFA_restore        = 0xc0,  //  register
    DW_CFA_nop            = 0x00,
    DW_CFA_set_loc        = 0x01,
    DW_CFA_advance_loc1   = 0x02,
    DW_CFA_advance_loc2   = 0x03,
    DW_CFA_advance_loc4   = 0x04,
    DW_CFA_offset_extended = 0x05,
    DW_CFA_restore_extended = 0x06,
    DW_CFA_undefined      = 0x07,
    DW_CFA_same_value     = 0x08,
    DW_CFA_register       = 0x09,
    DW_CFA_remember_state = 0x0a,
    DW_CFA_restore_state  = 0x0b,
    DW_CFA_def_cfa        = 0x0c,
    DW_CFA_def_cfa_register = 0x0d,
    DW_CFA_def_cfa_offset = 0x0e,
    DW_CFA_def_cfa_expression = 0x0f,
    DW_CFA_expression     = 0x10,
    DW_CFA_offset_extended_sf = 0x11,
    DW_CFA_def_cfa_sf     = 0x12,
    DW_CFA_def_cfa_offset_sf = 0x13,
    DW_CFA_val_offset     = 0x14,
    DW_CFA_val_offset_sf  = 0x15,
    DW_CFA_val_expression = 0x16,
};
```

---

## 高级主题

### 分离式 DWARF（Split DWARF）

DWARF 5 支持将调试信息分离到独立的 .dwo 文件中，减小可执行文件大小：

```bash
# 生成分离式 DWARF
gcc -gsplit-dwarf -c main.c -o main.o
# 生成 main.o 和 main.dwo

# 链接时打包为 .dwp
dwp main1.dwo main2.dwo -o debug.dwp

# 或使用 GNU 索引
# .debug_info -> .debug_info.dwo
# 新增节：.debug_cu_index, .debug_tu_index
```

### 调试信息压缩

```bash
# 使用 zlib 压缩 DWARF
# GCC: -gz=zlib
# Clang: -gdwarf-5 -gz

# 手动压缩/解压缩
eu-objcopy --compress-debug-sections=zlib binary
eu-objcopy --decompress-debug-sections binary
```

### 参考资料

1. DWARF Debugging Information Format Version 5
2. ELF-64 Object File Format
3. libdw 文档: https://sourceware.org/elfutils/
4. "Linkers and Loaders" by John R. Levine
