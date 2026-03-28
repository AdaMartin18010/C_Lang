/**
 * RISC-V Assembler - Complete Implementation
 * ==========================================
 * 
 * 一个教学用的RISC-V RV32I汇编器实现
 * 
 * 支持的特性:
 * - RV32I基础指令集 (R/I/S/B/U/J型)
 * - 32个通用寄存器 (x0-x31) 及其ABI名称
 * - 常见伪指令 (li, mv, nop, la)
 * - 符号表和两遍扫描算法
 * - 简单二进制输出格式
 * 
 * 使用方法:
 *   ./riscv_assembler input.s [-o output.bin]
 *   ./riscv_assembler input.s -t    # 文本格式输出
 * 
 * 配套文档: knowledge/22_Assembler_Implementation/02_RISCV_Assembler.md
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdarg.h>

#define MAX_LINE 512
#define MAX_SYMBOLS 1024
#define MAX_CODE 65536
#define MAX_TOKENS 10

/* 默认段地址 */
#define TEXT_START 0x1000
#define DATA_START 0x2000

/* ============================================================================
 * 数据类型定义
 * ============================================================================ */

/**
 * 指令类型枚举
 */
typedef enum {
    TYPE_R,      // 寄存器-寄存器
    TYPE_I,      // 立即数
    TYPE_S,      // 存储
    TYPE_B,      // 分支
    TYPE_U,      // 高位立即数
    TYPE_J,      // 跳转
    TYPE_PSEUDO  // 伪指令
} InstType;

/**
 * 符号表条目
 */
typedef struct {
    char name[64];
    uint32_t address;
    int type;      // 0=label, 1=variable
    int section;   // 0=.text, 1=.data
} Symbol;

/**
 * 指令编码信息
 */
typedef struct {
    const char *mnemonic;  // 助记符
    uint8_t opcode;        // 操作码
    uint8_t funct3;        // funct3字段
    uint8_t funct7;        // funct7字段（R型）
    InstType type;         // 指令类型
} InstructionInfo;

/**
 * 寄存器信息
 */
typedef struct {
    const char *name;
    uint8_t number;
} RegisterInfo;

/* ============================================================================
 * 全局变量
 * ============================================================================ */

static Symbol symtab[MAX_SYMBOLS];
static int sym_count = 0;
static uint32_t current_addr = TEXT_START;
static int current_section = 0;  // 0=.text, 1=.data
static int line_number = 0;
static int error_count = 0;

/* 编码后的机器码 */
static uint32_t code[MAX_CODE];
static int code_count = 0;

/* ============================================================================
 * 寄存器表
 * ============================================================================ */

static RegisterInfo reg_table[] = {
    // x寄存器名称
    {"x0", 0},   {"x1", 1},   {"x2", 2},   {"x3", 3},
    {"x4", 4},   {"x5", 5},   {"x6", 6},   {"x7", 7},
    {"x8", 8},   {"x9", 9},   {"x10", 10}, {"x11", 11},
    {"x12", 12}, {"x13", 13}, {"x14", 14}, {"x15", 15},
    {"x16", 16}, {"x17", 17}, {"x18", 18}, {"x19", 19},
    {"x20", 20}, {"x21", 21}, {"x22", 22}, {"x23", 23},
    {"x24", 24}, {"x25", 25}, {"x26", 26}, {"x27", 27},
    {"x28", 28}, {"x29", 29}, {"x30", 30}, {"x31", 31},
    // ABI名称
    {"zero", 0},  // 零寄存器
    {"ra", 1},    // 返回地址
    {"sp", 2},    // 栈指针
    {"gp", 3},    // 全局指针
    {"tp", 4},    // 线程指针
    {"t0", 5},    // 临时寄存器0
    {"t1", 6},    // 临时寄存器1
    {"t2", 7},    // 临时寄存器2
    {"s0", 8},    {"fp", 8},   // 保存寄存器0/帧指针
    {"s1", 9},    // 保存寄存器1
    {"a0", 10},   {"a1", 11},  // 函数参数0-1/返回值0-1
    {"a2", 12},   {"a3", 13},  // 函数参数2-3
    {"a4", 14},   {"a5", 15},  // 函数参数4-5
    {"a6", 16},   {"a7", 17},  // 函数参数6-7
    {"s2", 18},   {"s3", 19},  // 保存寄存器2-3
    {"s4", 20},   {"s5", 21},  // 保存寄存器4-5
    {"s6", 22},   {"s7", 23},  // 保存寄存器6-7
    {"s8", 24},   {"s9", 25},  // 保存寄存器8-9
    {"s10", 26},  {"s11", 27}, // 保存寄存器10-11
    {"t3", 28},   {"t4", 29},  // 临时寄存器3-4
    {"t5", 30},   {"t6", 31},  // 临时寄存器5-6
    {NULL, 0}
};

/* ============================================================================
 * 指令编码表
 * ============================================================================ */

static InstructionInfo inst_table[] = {
    /* R型指令 - 寄存器操作 */
    {"add",   0x33, 0x0, 0x00, TYPE_R},
    {"sub",   0x33, 0x0, 0x20, TYPE_R},
    /* R型指令 - M扩展 (乘除法) */
    {"mul",   0x33, 0x0, 0x01, TYPE_R},
    {"mulh",  0x33, 0x1, 0x01, TYPE_R},
    {"mulhsu",0x33, 0x2, 0x01, TYPE_R},
    {"mulhu", 0x33, 0x3, 0x01, TYPE_R},
    {"div",   0x33, 0x4, 0x01, TYPE_R},
    {"divu",  0x33, 0x5, 0x01, TYPE_R},
    {"rem",   0x33, 0x6, 0x01, TYPE_R},
    {"remu",  0x33, 0x7, 0x01, TYPE_R},
    {"sll",   0x33, 0x1, 0x00, TYPE_R},
    {"slt",   0x33, 0x2, 0x00, TYPE_R},
    {"sltu",  0x33, 0x3, 0x00, TYPE_R},
    {"xor",   0x33, 0x4, 0x00, TYPE_R},
    {"srl",   0x33, 0x5, 0x00, TYPE_R},
    {"sra",   0x33, 0x5, 0x20, TYPE_R},
    {"or",    0x33, 0x6, 0x00, TYPE_R},
    {"and",   0x33, 0x7, 0x00, TYPE_R},

    /* I型指令 - 算术立即数 */
    {"addi",  0x13, 0x0, 0x00, TYPE_I},
    {"slli",  0x13, 0x1, 0x00, TYPE_I},
    {"slti",  0x13, 0x2, 0x00, TYPE_I},
    {"sltui", 0x13, 0x3, 0x00, TYPE_I},
    {"xori",  0x13, 0x4, 0x00, TYPE_I},
    {"srli",  0x13, 0x5, 0x00, TYPE_I},
    {"srai",  0x13, 0x5, 0x20, TYPE_I},
    {"ori",   0x13, 0x6, 0x00, TYPE_I},
    {"andi",  0x13, 0x7, 0x00, TYPE_I},

    /* I型指令 - 加载 */
    {"lb",    0x03, 0x0, 0x00, TYPE_I},
    {"lh",    0x03, 0x1, 0x00, TYPE_I},
    {"lw",    0x03, 0x2, 0x00, TYPE_I},
    {"lbu",   0x03, 0x4, 0x00, TYPE_I},
    {"lhu",   0x03, 0x5, 0x00, TYPE_I},

    /* I型指令 - JALR */
    {"jalr",  0x67, 0x0, 0x00, TYPE_I},

    /* S型指令 - 存储 */
    {"sb",    0x23, 0x0, 0x00, TYPE_S},
    {"sh",    0x23, 0x1, 0x00, TYPE_S},
    {"sw",    0x23, 0x2, 0x00, TYPE_S},

    /* B型指令 - 分支 */
    {"beq",   0x63, 0x0, 0x00, TYPE_B},
    {"bne",   0x63, 0x1, 0x00, TYPE_B},
    {"blt",   0x63, 0x4, 0x00, TYPE_B},
    {"bge",   0x63, 0x5, 0x00, TYPE_B},
    {"bltu",  0x63, 0x6, 0x00, TYPE_B},
    {"bgeu",  0x63, 0x7, 0x00, TYPE_B},
    // 伪指令分支 (展开为相反条件的分支)
    {"ble",   0x63, 0x5, 0x00, TYPE_B},  // ble rs1, rs2, label -> bge rs2, rs1, label
    {"bgt",   0x63, 0x4, 0x00, TYPE_B},  // bgt rs1, rs2, label -> blt rs2, rs1, label

    /* U型指令 */
    {"lui",   0x37, 0x0, 0x00, TYPE_U},
    {"auipc", 0x17, 0x0, 0x00, TYPE_U},

    /* J型指令 */
    {"jal",   0x6F, 0x0, 0x00, TYPE_J},

    /* 伪指令 */
    {"li",    0x00, 0x0, 0x00, TYPE_PSEUDO},
    {"la",    0x00, 0x0, 0x00, TYPE_PSEUDO},
    {"mv",    0x00, 0x0, 0x00, TYPE_PSEUDO},
    {"nop",   0x00, 0x0, 0x00, TYPE_PSEUDO},
    {"j",     0x00, 0x0, 0x00, TYPE_PSEUDO},
    {"jr",    0x00, 0x0, 0x00, TYPE_PSEUDO},
    {"call",  0x00, 0x0, 0x00, TYPE_PSEUDO},
    {"ret",   0x00, 0x0, 0x00, TYPE_PSEUDO},

    {NULL, 0, 0, 0, 0}
};

/* ============================================================================
 * 工具函数
 * ============================================================================ */

void error(const char *fmt, ...) {
    va_list args;
    fprintf(stderr, "Error at line %d: ", line_number);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    error_count++;
}

void warning(const char *fmt, ...) {
    va_list args;
    fprintf(stderr, "Warning at line %d: ", line_number);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}

/**
 * 清理行内容
 */
void clean_line(char *line) {
    // 移除#注释
    char *comment = strchr(line, '#');
    if (comment) *comment = '\0';

    // 移除行尾空白和换行
    int len = strlen(line);
    while (len > 0 && isspace((unsigned char)line[len-1])) {
        line[--len] = '\0';
    }

    // 移除行首空白
    char *start = line;
    while (isspace((unsigned char)*start)) start++;
    if (start != line) memmove(line, start, strlen(start) + 1);
}

/**
 * 将字符串分割为tokens，保留括号以便偏移寻址解析
 */
int tokenize(char *line, char **tokens, int max_tokens) {
    int count = 0;
    char *p = line;
    
    while (*p && count < max_tokens) {
        // 跳过空白和逗号
        while (isspace((unsigned char)*p) || *p == ',') p++;
        if (!*p) break;
        
        // 读取token
        char *start = p;
        
        // 如果是偏移寻址格式如"4(x2)"，保留整个字符串
        if (isdigit((unsigned char)*p) || *p == '-' || *p == '+') {
            // 读取数字
            while (*p && (isdigit((unsigned char)*p) || *p == '-' || *p == '+' || 
                   (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F') || *p == 'x' || *p == 'X')) {
                p++;
            }
            // 检查是否有括号
            if (*p == '(') {
                while (*p && *p != ')') p++;
                if (*p == ')') p++;
            }
        } else {
            // 普通token
            while (*p && !isspace((unsigned char)*p) && *p != ',') {
                p++;
            }
        }
        
        int len = p - start;
        char *tok = malloc(len + 1);
        strncpy(tok, start, len);
        tok[len] = '\0';
        tokens[count++] = tok;
    }
    
    return count;
}

void free_tokens(char **tokens, int count) {
    for (int i = 0; i < count; i++) {
        free(tokens[i]);
        tokens[i] = NULL;
    }
}

/**
 * 查找寄存器编号
 */
int parse_register(const char *name) {
    for (int i = 0; reg_table[i].name != NULL; i++) {
        if (strcmp(reg_table[i].name, name) == 0) {
            return reg_table[i].number;
        }
    }
    return -1;
}

/**
 * 查找指令信息
 */
InstructionInfo* find_instruction(const char *mnemonic) {
    for (int i = 0; inst_table[i].mnemonic != NULL; i++) {
        if (strcmp(inst_table[i].mnemonic, mnemonic) == 0) {
            return &inst_table[i];
        }
    }
    return NULL;
}

/**
 * 查找符号
 */
int lookup_symbol(const char *name, uint32_t *addr) {
    for (int i = 0; i < sym_count; i++) {
        if (strcmp(symtab[i].name, name) == 0) {
            *addr = symtab[i].address;
            return 1;
        }
    }
    return 0;
}

/**
 * 添加符号
 */
void add_symbol(const char *name, uint32_t addr, int type) {
    if (sym_count >= MAX_SYMBOLS) {
        error("Symbol table full");
        return;
    }
    
    // 检查重复
    uint32_t existing;
    if (lookup_symbol(name, &existing)) {
        warning("Symbol '%s' redefined", name);
        return;
    }
    
    strncpy(symtab[sym_count].name, name, 63);
    symtab[sym_count].name[63] = '\0';
    symtab[sym_count].address = addr;
    symtab[sym_count].type = type;
    symtab[sym_count].section = current_section;
    sym_count++;
}

/**
 * 打印符号表
 */
void print_symbol_table(void) {
    printf("\n=== Symbol Table (%d entries) ===\n", sym_count);
    printf("%-20s %-10s %-10s %s\n", "Name", "Address", "Type", "Section");
    printf("------------------------------------------------\n");
    
    for (int i = 0; i < sym_count; i++) {
        const char *type_str = symtab[i].type == 0 ? "label" : "variable";
        const char *sect_str = symtab[i].section == 0 ? ".text" : ".data";
        printf("%-20s 0x%08X %-10s %s\n", 
               symtab[i].name, symtab[i].address, type_str, sect_str);
    }
    printf("===============================================\n\n");
}

/* ============================================================================
 * 指令编码函数
 * ============================================================================ */

/**
 * 编码R型指令
 * 
 * 格式: funct7 | rs2 | rs1 | funct3 | rd | opcode
 *       7      | 5   | 5   | 3      | 5  | 7
 */
uint32_t encode_r_type(uint8_t opcode, uint8_t funct3, uint8_t funct7,
                       uint8_t rd, uint8_t rs1, uint8_t rs2) {
    return ((uint32_t)funct7 << 25) |
           ((uint32_t)rs2 << 20) |
           ((uint32_t)rs1 << 15) |
           ((uint32_t)funct3 << 12) |
           ((uint32_t)rd << 7) |
           (uint32_t)opcode;
}

/**
 * 编码I型指令
 * 
 * 格式: imm[11:0] | rs1 | funct3 | rd | opcode
 *       12       | 5   | 3      | 5  | 7
 */
uint32_t encode_i_type(uint8_t opcode, uint8_t funct3,
                       uint8_t rd, uint8_t rs1, int32_t imm) {
    return (((uint32_t)imm & 0xFFF) << 20) |
           ((uint32_t)rs1 << 15) |
           ((uint32_t)funct3 << 12) |
           ((uint32_t)rd << 7) |
           (uint32_t)opcode;
}

/**
 * 编码S型指令
 * 
 * 格式: imm[11:5] | rs2 | rs1 | funct3 | imm[4:0] | opcode
 *       7        | 5   | 5   | 3      | 5        | 7
 */
uint32_t encode_s_type(uint8_t opcode, uint8_t funct3,
                       uint8_t rs1, uint8_t rs2, int32_t imm) {
    uint32_t imm11_5 = ((uint32_t)imm >> 5) & 0x7F;
    uint32_t imm4_0 = (uint32_t)imm & 0x1F;
    
    return (imm11_5 << 25) |
           ((uint32_t)rs2 << 20) |
           ((uint32_t)rs1 << 15) |
           ((uint32_t)funct3 << 12) |
           (imm4_0 << 7) |
           (uint32_t)opcode;
}

/**
 * 编码B型指令
 * 
 * 格式: imm[12|10:5] | rs2 | rs1 | funct3 | imm[4:1|11] | opcode
 *       7           | 5   | 5   | 3      | 5           | 7
 * 
 * 注意: imm是字节偏移，会被除以2编码
 */
uint32_t encode_b_type(uint8_t opcode, uint8_t funct3,
                       uint8_t rs1, uint8_t rs2, int32_t imm) {
    // B型使用字节偏移/2
    int32_t offset = imm >> 1;
    
    uint32_t imm12 = ((uint32_t)offset >> 11) & 0x1;
    uint32_t imm10_5 = ((uint32_t)offset >> 4) & 0x3F;
    uint32_t imm4_1 = ((uint32_t)offset >> 0) & 0xF;
    uint32_t imm11 = ((uint32_t)offset >> 3) & 0x1;
    
    return (imm12 << 31) |
           (imm10_5 << 25) |
           ((uint32_t)rs2 << 20) |
           ((uint32_t)rs1 << 15) |
           ((uint32_t)funct3 << 12) |
           (imm4_1 << 8) |
           (imm11 << 7) |
           (uint32_t)opcode;
}

/**
 * 编码U型指令
 * 
 * 格式: imm[31:12] | rd | opcode
 *       20        | 5  | 7
 */
uint32_t encode_u_type(uint8_t opcode, uint8_t rd, int32_t imm) {
    return (((uint32_t)imm & 0xFFFFF000)) |
           ((uint32_t)rd << 7) |
           (uint32_t)opcode;
}

/**
 * 编码J型指令
 * 
 * 格式: imm[20|10:1|11|19:12] | rd | opcode
 *       20                  | 5  | 7
 */
uint32_t encode_j_type(uint8_t opcode, uint8_t rd, int32_t imm) {
    int32_t offset = imm >> 1;  // J型也使用半字偏移
    
    uint32_t imm20 = ((uint32_t)offset >> 19) & 0x1;
    uint32_t imm10_1 = ((uint32_t)offset >> 0) & 0x3FF;
    uint32_t imm11 = ((uint32_t)offset >> 10) & 0x1;
    uint32_t imm19_12 = ((uint32_t)offset >> 11) & 0xFF;
    
    return (imm20 << 31) |
           (imm10_1 << 21) |
           (imm11 << 20) |
           (imm19_12 << 12) |
           ((uint32_t)rd << 7) |
           (uint32_t)opcode;
}

/* ============================================================================
 * 伪指令展开
 * ============================================================================ */

/**
 * 展开伪指令
 * @return 展开的指令数，0表示失败
 */
int expand_pseudo(const char *mnemonic, char **tokens, int token_count,
                  uint32_t pc, uint32_t *output) {
    
    if (strcmp(mnemonic, "nop") == 0) {
        // nop -> addi x0, x0, 0
        output[0] = encode_i_type(0x13, 0, 0, 0, 0);
        return 1;
    }
    
    if (strcmp(mnemonic, "mv") == 0) {
        // mv rd, rs -> addi rd, rs, 0
        if (token_count < 3) return 0;
        int rd = parse_register(tokens[1]);
        int rs1 = parse_register(tokens[2]);
        if (rd < 0 || rs1 < 0) return 0;
        output[0] = encode_i_type(0x13, 0, rd, rs1, 0);
        return 1;
    }
    
    if (strcmp(mnemonic, "li") == 0) {
        // li rd, immediate
        if (token_count < 3) return 0;
        int rd = parse_register(tokens[1]);
        if (rd < 0) return 0;
        
        int32_t imm = (int32_t)strtol(tokens[2], NULL, 0);
        
        if (imm >= -2048 && imm < 2048) {
            // 适合I型立即数: addi rd, x0, imm
            output[0] = encode_i_type(0x13, 0, rd, 0, imm);
            return 1;
        } else if ((imm & 0xFFF) == 0) {
            // 只有高20位: lui rd, hi20
            output[0] = encode_u_type(0x37, rd, imm);
            return 1;
        } else {
            // 需要两条指令: lui + addi
            int32_t hi = (imm >> 12) + ((imm >> 11) & 1);  // 调整符号
            int32_t lo = imm & 0xFFF;
            if (lo >= 2048) lo -= 4096;  // 符号扩展
            
            output[0] = encode_u_type(0x37, rd, hi << 12);      // lui
            output[1] = encode_i_type(0x13, 0, rd, rd, lo);     // addi
            return 2;
        }
    }
    
    if (strcmp(mnemonic, "j") == 0) {
        // j offset -> jal x0, offset
        if (token_count < 2) return 0;
        int32_t offset = (int32_t)strtol(tokens[1], NULL, 0);
        output[0] = encode_j_type(0x6F, 0, offset);
        return 1;
    }
    
    if (strcmp(mnemonic, "jr") == 0) {
        // jr rs -> jalr x0, rs, 0
        if (token_count < 2) return 0;
        int rs1 = parse_register(tokens[1]);
        if (rs1 < 0) return 0;
        output[0] = encode_i_type(0x67, 0, 0, rs1, 0);
        return 1;
    }
    
    if (strcmp(mnemonic, "ret") == 0) {
        // ret -> jalr x0, ra, 0
        output[0] = encode_i_type(0x67, 0, 0, 1, 0);
        return 1;
    }
    
    if (strcmp(mnemonic, "call") == 0) {
        // call symbol -> auipc ra, offset[31:12]; jalr ra, offset[11:0](ra)
        // 简化实现: 假设直接给出偏移
        if (token_count < 2) return 0;
        
        uint32_t target;
        int32_t offset;
        
        if (lookup_symbol(tokens[1], &target)) {
            offset = (int32_t)(target - pc);
        } else {
            offset = (int32_t)strtol(tokens[1], NULL, 0);
        }
        
        int32_t hi = ((offset + 0x800) >> 12) & 0xFFFFF;
        int32_t lo = offset & 0xFFF;
        if (lo >= 2048) lo -= 4096;
        
        output[0] = encode_u_type(0x17, 1, hi << 12);       // auipc ra, hi
        output[1] = encode_i_type(0x67, 0, 1, 1, lo);       // jalr ra, lo(ra)
        return 2;
    }
    
    return 0;
}

/* ============================================================================
 * 解析和编码
 * ============================================================================ */

/**
 * 解析存储指令的偏移寻址格式
 * 例如: 8(x2) -> offset=8, base=x2
 */
int parse_offset_addressing(const char *str, int32_t *offset, int *base_reg) {
    char *lparen = strchr(str, '(');
    if (!lparen) return 0;
    
    *lparen = '\0';
    *offset = (int32_t)strtol(str, NULL, 0);
    
    char *rparen = strchr(lparen + 1, ')');
    if (!rparen) return 0;
    *rparen = '\0';
    
    *base_reg = parse_register(lparen + 1);
    *lparen = '(';  // 恢复原字符串
    
    return *base_reg >= 0;
}

/**
 * 解析单条指令并编码
 * @return 编码的指令数，0表示失败
 */
int assemble_instruction(const char *line, uint32_t pc, uint32_t *output) {
    char buffer[MAX_LINE];
    strncpy(buffer, line, MAX_LINE - 1);
    buffer[MAX_LINE - 1] = '\0';
    
    char *tokens[MAX_TOKENS];
    int token_count = tokenize(buffer, tokens, MAX_TOKENS);
    
    if (token_count == 0) {
        free_tokens(tokens, token_count);
        return 0;
    }
    
    const char *mnemonic = tokens[0];
    
    // 检查是否为伪指令
    if (strcmp(mnemonic, "nop") == 0 ||
        strcmp(mnemonic, "mv") == 0 ||
        strcmp(mnemonic, "li") == 0 ||
        strcmp(mnemonic, "j") == 0 ||
        strcmp(mnemonic, "jr") == 0 ||
        strcmp(mnemonic, "ret") == 0 ||
        strcmp(mnemonic, "call") == 0) {
        int n = expand_pseudo(mnemonic, tokens, token_count, pc, output);
        free_tokens(tokens, token_count);
        return n;
    }
    
    // 查找指令信息
    InstructionInfo *info = find_instruction(mnemonic);
    if (!info) {
        error("Unknown instruction: %s", mnemonic);
        free_tokens(tokens, token_count);
        return 0;
    }
    
    int result = 0;
    
    switch (info->type) {
        case TYPE_R: {
            // R型: op rd, rs1, rs2
            if (token_count < 4) {
                error("R-type needs 3 operands");
                break;
            }
            int rd = parse_register(tokens[1]);
            int rs1 = parse_register(tokens[2]);
            int rs2 = parse_register(tokens[3]);
            if (rd < 0 || rs1 < 0 || rs2 < 0) {
                error("Invalid register");
                break;
            }
            output[0] = encode_r_type(info->opcode, info->funct3, info->funct7,
                                       rd, rs1, rs2);
            result = 1;
            break;
        }
        
        case TYPE_I: {
            // I型分为两类:
            // - 算术: addi rd, rs1, imm
            // - 加载: lw rd, offset(rs1)
            // - jalr: jalr rd, offset(rs1)
            
            if (strcmp(mnemonic, "lb") == 0 || strcmp(mnemonic, "lh") == 0 ||
                strcmp(mnemonic, "lw") == 0 || strcmp(mnemonic, "lbu") == 0 ||
                strcmp(mnemonic, "lhu") == 0) {
                // 加载格式: op rd, offset(rs1)
                if (token_count < 3) {
                    error("I-type load needs 2 operands");
                    break;
                }
                int rd = parse_register(tokens[1]);
                int32_t offset;
                int rs1;
                if (!parse_offset_addressing(tokens[2], &offset, &rs1)) {
                    error("Invalid offset addressing");
                    break;
                }
                output[0] = encode_i_type(info->opcode, info->funct3, rd, rs1, offset);
                result = 1;
            } else if (strcmp(mnemonic, "jalr") == 0) {
                // jalr格式: jalr rd, rs1, imm 或 jalr rd, offset(rs1)
                if (token_count < 3) {
                    error("jalr needs at least 2 operands");
                    break;
                }
                int rd = parse_register(tokens[1]);
                int32_t offset;
                int rs1;
                
                // 尝试offset(rs1)格式
                if (parse_offset_addressing(tokens[2], &offset, &rs1)) {
                    output[0] = encode_i_type(info->opcode, info->funct3, rd, rs1, offset);
                    result = 1;
                } else {
                    // jalr rd, rs1, imm 格式
                    rs1 = parse_register(tokens[2]);
                    if (token_count >= 4) {
                        offset = (int32_t)strtol(tokens[3], NULL, 0);
                    } else {
                        offset = 0;
                    }
                    if (rs1 < 0) {
                        error("Invalid register");
                        break;
                    }
                    output[0] = encode_i_type(info->opcode, info->funct3, rd, rs1, offset);
                    result = 1;
                }
            } else {
                // 算术格式: op rd, rs1, imm
                if (token_count < 4) {
                    error("I-type needs 3 operands");
                    break;
                }
                int rd = parse_register(tokens[1]);
                int rs1 = parse_register(tokens[2]);
                int32_t imm = (int32_t)strtol(tokens[3], NULL, 0);
                if (rd < 0 || rs1 < 0) {
                    error("Invalid register");
                    break;
                }
                // 检查立即数范围
                if (imm < -2048 || imm >= 2048) {
                    warning("I-type immediate out of range: %d", imm);
                }
                output[0] = encode_i_type(info->opcode, info->funct3, rd, rs1, imm);
                result = 1;
            }
            break;
        }
        
        case TYPE_S: {
            // S型: sw rs2, offset(rs1)
            if (token_count < 3) {
                error("S-type needs 2 operands");
                break;
            }
            int rs2 = parse_register(tokens[1]);
            int32_t offset;
            int rs1;
            if (!parse_offset_addressing(tokens[2], &offset, &rs1)) {
                error("Invalid offset addressing");
                break;
            }
            output[0] = encode_s_type(info->opcode, info->funct3, rs1, rs2, offset);
            result = 1;
            break;
        }
        
        case TYPE_B: {
            // B型: beq rs1, rs2, label/offset
            // 伪指令: ble/bgt 需要交换操作数
            if (token_count < 4) {
                error("B-type needs 3 operands");
                break;
            }
            int rs1 = parse_register(tokens[1]);
            int rs2 = parse_register(tokens[2]);
            if (rs1 < 0 || rs2 < 0) {
                error("Invalid register");
                break;
            }
            
            // 处理伪指令: ble -> bge (交换操作数)
            if (strcmp(mnemonic, "ble") == 0) {
                int temp = rs1; rs1 = rs2; rs2 = temp;
            }
            // 处理伪指令: bgt -> blt (交换操作数)
            else if (strcmp(mnemonic, "bgt") == 0) {
                int temp = rs1; rs1 = rs2; rs2 = temp;
            }
            
            // 解析目标
            int32_t offset;
            uint32_t target;
            if (lookup_symbol(tokens[3], &target)) {
                offset = (int32_t)(target - pc);
            } else {
                offset = (int32_t)strtol(tokens[3], NULL, 0);
            }
            
            output[0] = encode_b_type(info->opcode, info->funct3, rs1, rs2, offset);
            result = 1;
            break;
        }
        
        case TYPE_U: {
            // U型: lui rd, imm
            if (token_count < 3) {
                error("U-type needs 2 operands");
                break;
            }
            int rd = parse_register(tokens[1]);
            int32_t imm = (int32_t)strtol(tokens[2], NULL, 0);
            if (rd < 0) {
                error("Invalid register");
                break;
            }
            output[0] = encode_u_type(info->opcode, rd, imm);
            result = 1;
            break;
        }
        
        case TYPE_J: {
            // J型: jal rd, label/offset
            if (token_count < 3) {
                error("J-type needs 2 operands");
                break;
            }
            int rd = parse_register(tokens[1]);
            if (rd < 0) {
                error("Invalid register");
                break;
            }
            
            int32_t offset;
            uint32_t target;
            if (lookup_symbol(tokens[2], &target)) {
                offset = (int32_t)(target - pc);
            } else {
                offset = (int32_t)strtol(tokens[2], NULL, 0);
            }
            
            output[0] = encode_j_type(info->opcode, rd, offset);
            result = 1;
            break;
        }
        
        default:
            error("Unhandled instruction type");
            break;
    }
    
    free_tokens(tokens, token_count);
    return result;
}

/* ============================================================================
 * 两遍扫描
 * ============================================================================ */

/**
 * 检查是否是伪指令/指令
 */
int is_instruction(const char *line) {
    // 跳过空行、注释、标签、段指令
    if (strlen(line) == 0) return 0;
    if (line[0] == '#' || line[0] == '.' || line[strlen(line)-1] == ':') return 0;
    return 1;
}

/**
 * 第一遍：构建符号表
 */
void first_pass(FILE *fp) {
    char line[MAX_LINE];
    current_addr = TEXT_START;
    current_section = 0;
    line_number = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        line_number++;
        clean_line(line);
        
        // 检查段指令
        if (strcmp(line, ".text") == 0) {
            current_section = 0;
            current_addr = TEXT_START;
            continue;
        }
        if (strcmp(line, ".data") == 0) {
            current_section = 1;
            current_addr = DATA_START;
            continue;
        }
        if (strncmp(line, ".globl", 6) == 0) {
            continue;  // 忽略全局声明
        }
        
        // 检查标签
        int len = strlen(line);
        if (len > 0 && line[len-1] == ':') {
            line[len-1] = '\0';
            add_symbol(line, current_addr, 0);
            continue;
        }
        
        // 计算指令大小（用于地址计算）
        if (is_instruction(line)) {
            // 伪指令可能需要多条指令
            char *tokens[MAX_TOKENS];
            char buffer[MAX_LINE];
            strncpy(buffer, line, MAX_LINE - 1);
            buffer[MAX_LINE - 1] = '\0';
            int token_count = tokenize(buffer, tokens, MAX_TOKENS);
            
            if (token_count > 0) {
                if (strcmp(tokens[0], "li") == 0 && token_count >= 3) {
                    int32_t imm = (int32_t)strtol(tokens[2], NULL, 0);
                    if (imm >= -2048 && imm < 2048) {
                        current_addr += 4;  // 1条指令
                    } else {
                        current_addr += 8;  // 2条指令
                    }
                } else if (strcmp(tokens[0], "call") == 0 ||
                           strcmp(tokens[0], "la") == 0) {
                    current_addr += 8;  // 2条指令
                } else {
                    current_addr += 4;  // 1条指令
                }
            }
            free_tokens(tokens, token_count);
        }
    }
    
    rewind(fp);
}

/**
 * 第二遍：生成机器码
 */
void second_pass(FILE *fp, FILE *out, int text_mode) {
    char line[MAX_LINE];
    current_addr = TEXT_START;
    current_section = 0;
    line_number = 0;
    code_count = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        line_number++;
        clean_line(line);
        
        // 跳过空行和标签
        if (strlen(line) == 0) continue;
        if (line[strlen(line)-1] == ':') continue;
        
        // 跳过段指令
        if (strcmp(line, ".text") == 0) {
            current_section = 0;
            current_addr = TEXT_START;
            continue;
        }
        if (strcmp(line, ".data") == 0) {
            current_section = 1;
            current_addr = DATA_START;
            continue;
        }
        if (strncmp(line, ".globl", 6) == 0) continue;
        
        // 汇编指令
        uint32_t instructions[4];  // 最多4条展开
        int n = assemble_instruction(line, current_addr, instructions);
        
        if (n > 0) {
            for (int i = 0; i < n; i++) {
                if (code_count >= MAX_CODE) {
                    error("Code buffer full");
                    return;
                }
                code[code_count++] = instructions[i];
                
                if (text_mode) {
                    fprintf(out, "0x%08X: 0x%08X    # %s\n", 
                            current_addr, instructions[i], line);
                }
                current_addr += 4;
            }
        } else if (n == 0 && is_instruction(line)) {
            // 有错误但继续
        }
    }
    
    // 如果不是文本模式，输出二进制
    if (!text_mode) {
        fwrite(code, sizeof(uint32_t), code_count, out);
    }
}

/* ============================================================================
 * 主函数
 * ============================================================================ */

void print_usage(const char *program) {
    fprintf(stderr, "RISC-V RV32I Assembler\n");
    fprintf(stderr, "Usage: %s <input.s> [options]\n\n", program);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -o <file>    指定输出文件（默认: output.bin）\n");
    fprintf(stderr, "  -t           文本格式输出（默认: 二进制）\n");
    fprintf(stderr, "  -s           显示符号表\n");
    fprintf(stderr, "  -h           显示帮助\n");
}

int main(int argc, char *argv[]) {
    char *input_file = NULL;
    char *output_file = NULL;
    int text_mode = 0;
    int show_symbols = 0;
    
    // 解析参数
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_file = argv[++i];
        } else if (strcmp(argv[i], "-t") == 0) {
            text_mode = 1;
        } else if (strcmp(argv[i], "-s") == 0) {
            show_symbols = 1;
        } else if (strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (argv[i][0] != '-') {
            input_file = argv[i];
        }
    }
    
    if (!input_file) {
        print_usage(argv[0]);
        return 1;
    }
    
    // 默认输出文件名
    char default_output[256];
    if (!output_file) {
        if (text_mode) {
            snprintf(default_output, sizeof(default_output), "%s.txt", input_file);
        } else {
            snprintf(default_output, sizeof(default_output), "%s.bin", input_file);
        }
        output_file = default_output;
    }
    
    printf("RISC-V Assembler\n");
    printf("Input:  %s\n", input_file);
    printf("Output: %s (%s)\n", output_file, text_mode ? "text" : "binary");
    printf("\n");
    
    // 打开输入
    FILE *in = fopen(input_file, "r");
    if (!in) {
        perror("Cannot open input file");
        return 1;
    }
    
    // 第一遍
    printf("First pass: Building symbol table...\n");
    first_pass(in);
    printf("  Symbols: %d\n", sym_count);
    
    if (show_symbols) {
        print_symbol_table();
    }
    
    // 第二遍
    printf("\nSecond pass: Generating code...\n");
    FILE *out = fopen(output_file, text_mode ? "w" : "wb");
    if (!out) {
        perror("Cannot open output file");
        fclose(in);
        return 1;
    }
    
    second_pass(in, out, text_mode);
    
    printf("  Instructions generated: %d\n", code_count);
    printf("  Code size: %d bytes\n", code_count * 4);
    
    if (error_count > 0) {
        printf("  Errors: %d\n", error_count);
    }
    
    printf("\nAssembly %s\n", error_count == 0 ? "successful" : "completed with errors");
    
    fclose(in);
    fclose(out);
    
    return error_count > 0 ? 1 : 0;
}
