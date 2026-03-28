/**
 * Hack Assembler - Complete Implementation
 * =========================================
 * 
 * 基于Nand2Tetris课程的Hack计算机汇编器实现
 * 
 * 特性:
 * - 支持A指令（地址/立即数）
 * - 支持C指令（计算、目标、跳转）
 * - 支持标签符号 (LABEL)
 * - 支持变量符号
 * - 两遍扫描算法
 * - 完整的符号表实现
 * 
 * 使用方法:
 *   ./hack_assembler input.asm
 * 输出:
 *   input.hack (二进制文件，每行16位)
 * 
 * 配套文档: knowledge/22_Assembler_Implementation/01_Hack_Assembler.md
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define MAX_LINE 256
#define MAX_SYMBOLS 1024
#define MAX_PROGRAM 32768
#define ROM_START 0
#define RAM_START 16  // 变量从RAM 16开始分配

/* ============================================================================
 * 数据结构定义
 * ============================================================================ */

/**
 * 符号表条目
 * 存储符号名称和对应的地址
 */
typedef struct {
    char name[MAX_LINE];
    int address;
} SymbolEntry;

/**
 * 符号表
 * 使用简单数组实现，适合教学目的
 * 工业级实现会使用哈希表
 */
typedef struct {
    SymbolEntry entries[MAX_SYMBOLS];
    int count;
} SymbolTable;

/* ============================================================================
 * 符号表操作
 * ============================================================================ */

/**
 * 初始化符号表
 * 
 * 预定义符号包括:
 * - R0-R15: 寄存器0-15 (地址0-15)
 * - SP: 栈指针 (地址0)
 * - LCL: 局部段基址 (地址1)
 * - ARG: 参数段基址 (地址2)
 * - THIS: this段基址 (地址3)
 * - THAT: that段基址 (地址4)
 * - SCREEN: 屏幕内存映射 (地址16384)
 * - KBD: 键盘寄存器 (地址24576)
 */
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
        {"SP", "0"}, 
        {"LCL", "1"}, 
        {"ARG", "2"},
        {"THIS", "3"}, 
        {"THAT", "4"},
        {"SCREEN", "16384"}, 
        {"KBD", "24576"},
        {NULL, NULL}
    };

    for (int i = 0; predefined[i][0] != NULL; i++) {
        strcpy(table->entries[table->count].name, predefined[i][0]);
        table->entries[table->count].address = atoi(predefined[i][1]);
        table->count++;
    }
}

/**
 * 查找符号
 * @return 1表示找到，0表示未找到
 */
int lookup_symbol(SymbolTable *table, const char *name, int *address) {
    for (int i = 0; i < table->count; i++) {
        if (strcmp(table->entries[i].name, name) == 0) {
            *address = table->entries[i].address;
            return 1;  // 找到
        }
    }
    return 0;  // 未找到
}

/**
 * 添加符号到符号表
 */
void add_symbol(SymbolTable *table, const char *name, int address) {
    if (table->count >= MAX_SYMBOLS) {
        fprintf(stderr, "Error: Symbol table full\n");
        exit(1);
    }
    
    // 检查是否已存在
    int existing_addr;
    if (lookup_symbol(table, name, &existing_addr)) {
        fprintf(stderr, "Warning: Symbol '%s' already defined at address %d\n", 
                name, existing_addr);
        return;
    }
    
    strcpy(table->entries[table->count].name, name);
    table->entries[table->count].address = address;
    table->count++;
}

/**
 * 打印符号表（用于调试）
 */
void print_symbol_table(SymbolTable *table) {
    printf("\n=== Symbol Table (%d entries) ===\n", table->count);
    for (int i = 0; i < table->count; i++) {
        printf("  %-10s = %5d (0x%04X)\n", 
               table->entries[i].name,
               table->entries[i].address,
               table->entries[i].address);
    }
    printf("================================\n\n");
}

/* ============================================================================
 * 词法分析辅助函数
 * ============================================================================ */

/**
 * 清理行内容
 * - 移除 // 注释
 * - 移除行首和行尾空白
 * - 结果存储在原缓冲区
 */
void clean_line(char *line) {
    // 移除//注释
    char *comment = strstr(line, "//");
    if (comment) *comment = '\0';

    // 移除行尾空白（包括\r和\n）
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
 * 检查字符串是否为纯数字（十进制）
 */
int is_number(const char *str) {
    if (*str == '\0') return 0;
    while (*str) {
        if (!isdigit((unsigned char)*str)) return 0;
        str++;
    }
    return 1;
}

/* ============================================================================
 * 指令解析
 * ============================================================================ */

/**
 * 解析A指令: @value
 * @value可以是:
 *   - 十进制常数: @21
 *   - 符号: @SYMBOL, @LOOP
 *   - 预定义符号: @R0, @SCREEN, @KBD
 * @return 1表示成功解析，0表示不是A指令
 */
int parse_a_instruction(const char *line, char *symbol) {
    if (line[0] != '@') return 0;
    strcpy(symbol, line + 1);
    return 1;
}

/**
 * 解析标签定义: (LABEL)
 * 标签用于标记代码位置，供跳转指令引用
 * @return 1表示成功解析，0表示不是标签
 */
int parse_label(const char *line, char *label) {
    if (line[0] != '(') return 0;

    const char *end = strchr(line, ')');
    if (!end) return 0;

    int len = (int)(end - line - 1);
    if (len <= 0 || len >= MAX_LINE) return 0;
    
    strncpy(label, line + 1, len);
    label[len] = '\0';
    return 1;
}

/**
 * 解析C指令: dest=comp;jump
 * 
 * C指令有三种形式:
 *   dest=comp;jump  (完整形式)
 *   dest=comp       (无跳转)
 *   comp;jump       (无目标)
 *   comp            (无目标无跳转)
 * 
 * @return 1表示成功解析
 */
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
        // 形式: dest=comp 或 dest=comp;jump
        *equal = '\0';
        strcpy(dest, temp);

        if (semi) {
            // 形式: dest=comp;jump
            *semi = '\0';
            strcpy(comp, equal + 1);
            strcpy(jump, semi + 1);
        } else {
            // 形式: dest=comp
            strcpy(comp, equal + 1);
        }
    } else if (semi) {
        // 形式: comp;jump
        *semi = '\0';
        strcpy(comp, temp);
        strcpy(jump, semi + 1);
    } else {
        // 形式: comp
        strcpy(comp, temp);
    }

    return 1;
}

/* ============================================================================
 * 指令编码
 * ============================================================================ */

/**
 * C指令计算字段(comp)编码表
 * 
 * Hack C指令格式: 111 a c1c2c3c4c5c6 d1d2d3 j1j2j3
 * 
 * comp字段: a c1c2c3c4c5c6 (7位)
 * - a位: 0表示使用A寄存器，1表示使用M（内存[A]）
 */

// comp编码表: 助记符 -> 编码(a+6位)
// 注意: 包含M的版本a位为1，但表中存储的是a=0的编码
const char *comp_table[][2] = {
    // 基础计算 (a=0)
    {"0",   "0101010"},
    {"1",   "0111111"},
    {"-1",  "0111010"},
    {"D",   "0001100"},
    {"A",   "0110000"},
    {"!D",  "0001101"},
    {"!A",  "0110001"},
    {"-D",  "0001111"},
    {"-A",  "0110011"},
    {"D+1", "0011111"},
    {"A+1", "0110111"},
    {"D-1", "0001110"},
    {"A-1", "0110010"},
    {"D+A", "0000010"},
    {"D-A", "0010011"},
    {"A-D", "0000111"},
    {"D&A", "0000000"},
    {"D|A", "0010101"},
    // M版本 (与A版本相同，但a=1)
    {"M",   "1110000"},   // a=1, 其余同A
    {"!M",  "1110001"},
    {"-M",  "1110011"},
    {"M+1", "1110111"},
    {"M-1", "1110010"},
    {"D+M", "1000010"},
    {"D-M", "1010011"},
    {"M-D", "1000111"},
    {"D&M", "1000000"},
    {"D|M", "1010101"},
    {NULL, NULL}
};

/**
 * 编码comp字段
 * @return 1表示成功，0表示未知comp
 */
int encode_comp(const char *comp, char *binary) {
    for (int i = 0; comp_table[i][0] != NULL; i++) {
        if (strcmp(comp_table[i][0], comp) == 0) {
            strcpy(binary, comp_table[i][1]);
            return 1;
        }
    }
    return 0;
}

/**
 * 编码dest字段 (3位: d1 d2 d3)
 * 
 * d1=1: 写入A寄存器
 * d2=1: 写入D寄存器
 * d3=1: 写入M（内存[A]）
 */
int encode_dest(const char *dest, char *binary) {
    int d1 = strchr(dest, 'A') ? 1 : 0;
    int d2 = strchr(dest, 'D') ? 1 : 0;
    int d3 = strchr(dest, 'M') ? 1 : 0;
    sprintf(binary, "%d%d%d", d1, d2, d3);
    return 1;
}

/**
 * 编码jump字段 (3位: j1 j2 j3)
 * 
 * j1: 如果out<0则跳转
 * j2: 如果out=0则跳转
 * j3: 如果out>0则跳转
 */
int encode_jump(const char *jump, char *binary) {
    if (strlen(jump) == 0) {
        strcpy(binary, "000");
    } else if (strcmp(jump, "JGT") == 0) strcpy(binary, "001");  // out > 0
    else if (strcmp(jump, "JEQ") == 0) strcpy(binary, "010");  // out = 0
    else if (strcmp(jump, "JGE") == 0) strcpy(binary, "011");  // out >= 0
    else if (strcmp(jump, "JLT") == 0) strcpy(binary, "100");  // out < 0
    else if (strcmp(jump, "JNE") == 0) strcpy(binary, "101");  // out != 0
    else if (strcmp(jump, "JLE") == 0) strcpy(binary, "110");  // out <= 0
    else if (strcmp(jump, "JMP") == 0) strcpy(binary, "111");  // 无条件
    else return 0;
    return 1;
}

/* ============================================================================
 * 两遍扫描算法
 * ============================================================================ */

/**
 * 第一遍：构建符号表
 * 
 * 目标:
 * 1. 扫描所有标签定义 (LABEL)
 * 2. 记录每个标签对应的ROM地址
 * 3. 统计指令数量
 * 
 * 注意:
 * - 标签本身不占ROM空间
 * - ROM地址只计算实际指令（A指令和C指令）
 */
void first_pass(FILE *input, SymbolTable *table, int *instruction_count) {
    char line[MAX_LINE];
    char symbol[MAX_LINE];
    int rom_address = ROM_START;

    while (fgets(line, sizeof(line), input)) {
        clean_line(line);
        if (strlen(line) == 0) continue;

        if (parse_label(line, symbol)) {
            // 标签定义，记录当前地址
            add_symbol(table, symbol, rom_address);
            if (getenv("HACK_DEBUG")) {
                printf("[First Pass] Label '%s' = %d\n", symbol, rom_address);
            }
        } else {
            // 普通指令（A或C），地址+1
            rom_address++;
        }
    }

    *instruction_count = rom_address;
    rewind(input);
}

/**
 * 第二遍：生成机器码
 * 
 * 目标:
 * 1. 将每条指令翻译为16位二进制
 * 2. 解析符号引用（变量和标签）
 * 3. 为新变量分配RAM地址（从16开始）
 * 4. 输出.hack文件
 */
void second_pass(FILE *input, FILE *output, SymbolTable *table, int *a_count, int *c_count) {
    char line[MAX_LINE];
    char symbol[MAX_LINE];
    char dest[MAX_LINE], comp[MAX_LINE], jump[MAX_LINE];
    int next_ram = RAM_START;  // 变量从RAM 16开始分配
    int line_num = 0;

    *a_count = 0;
    *c_count = 0;

    while (fgets(line, sizeof(line), input)) {
        line_num++;
        clean_line(line);
        if (strlen(line) == 0) continue;

        // 跳过标签定义（已在第一遍处理）
        if (parse_label(line, symbol)) continue;

        // ===== A指令处理 =====
        if (parse_a_instruction(line, symbol)) {
            int value;
            
            if (is_number(symbol)) {
                // 常数直接转换
                value = atoi(symbol);
                if (value < 0 || value > 32767) {
                    fprintf(stderr, "Error line %d: A-instruction value out of range: %d\n", 
                            line_num, value);
                    continue;
                }
            } else {
                // 符号查找或创建变量
                if (!lookup_symbol(table, symbol, &value)) {
                    // 新变量，分配RAM地址
                    value = next_ram++;
                    add_symbol(table, symbol, value);
                    if (getenv("HACK_DEBUG")) {
                        printf("[Second Pass] Variable '%s' allocated at RAM %d\n", 
                               symbol, value);
                    }
                }
            }
            
            // 输出A指令: 0 + 15位值
            // A指令最高位为0，其余15位为值
            fprintf(output, "%016d\n", (int)(value & 0x7FFF));
            (*a_count)++;
        }
        // ===== C指令处理 =====
        else if (parse_c_instruction(line, dest, comp, jump)) {
            char comp_bits[8], dest_bits[4], jump_bits[4];

            if (!encode_comp(comp, comp_bits)) {
                fprintf(stderr, "Error line %d: Unknown comp: '%s'\n", line_num, comp);
                continue;
            }
            encode_dest(dest, dest_bits);
            encode_jump(jump, jump_bits);

            // C指令格式: 111 + comp(7) + dest(3) + jump(3)
            fprintf(output, "111%s%s%s\n", comp_bits, dest_bits, jump_bits);
            (*c_count)++;
        }
        else {
            fprintf(stderr, "Error line %d: Unknown instruction: '%s'\n", line_num, line);
        }
    }
}

/* ============================================================================
 * 主函数
 * ============================================================================ */

void print_usage(const char *program) {
    fprintf(stderr, "Usage: %s <input.asm> [options]\n", program);
    fprintf(stderr, "\nOptions:\n");
    fprintf(stderr, "  -o <file>    指定输出文件（默认: input.hack）\n");
    fprintf(stderr, "  -v           显示详细信息\n");
    fprintf(stderr, "  -s           显示符号表\n");
    fprintf(stderr, "\nEnvironment:\n");
    fprintf(stderr, "  HACK_DEBUG=1  启用调试输出\n");
}

int main(int argc, char *argv[]) {
    // 解析命令行参数
    char *input_name = NULL;
    char *output_name = NULL;
    int verbose = 0;
    int show_symbols = 0;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
                output_name = argv[++i];
            } else if (strcmp(argv[i], "-v") == 0) {
                verbose = 1;
            } else if (strcmp(argv[i], "-s") == 0) {
                show_symbols = 1;
            } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                print_usage(argv[0]);
                return 0;
            }
        } else {
            input_name = argv[i];
        }
    }

    if (!input_name) {
        print_usage(argv[0]);
        return 1;
    }

    // 生成输出文件名
    char default_output[MAX_LINE];
    if (!output_name) {
        strcpy(default_output, input_name);
        char *dot = strrchr(default_output, '.');
        if (dot) strcpy(dot, ".hack");
        else strcat(default_output, ".hack");
        output_name = default_output;
    }

    if (verbose) {
        printf("Hack Assembler\n");
        printf("Input:  %s\n", input_name);
        printf("Output: %s\n", output_name);
        printf("\n");
    }

    // 打开输入文件
    FILE *input = fopen(input_name, "r");
    if (!input) {
        perror("Error opening input file");
        return 1;
    }

    // 打开输出文件
    FILE *output = fopen(output_name, "w");
    if (!output) {
        perror("Error opening output file");
        fclose(input);
        return 1;
    }

    // 初始化符号表
    SymbolTable table;
    init_symbol_table(&table);

    // ============ 第一遍 ============
    int instruction_count;
    first_pass(input, &table, &instruction_count);
    
    if (verbose) {
        printf("First Pass Complete:\n");
        printf("  Instructions: %d\n", instruction_count);
        printf("  Symbols: %d\n", table.count);
    }

    // ============ 第二遍 ============
    int a_count, c_count;
    second_pass(input, output, &table, &a_count, &c_count);

    if (verbose) {
        printf("\nSecond Pass Complete:\n");
        printf("  A-instructions: %d\n", a_count);
        printf("  C-instructions: %d\n", c_count);
        printf("  Total: %d\n", a_count + c_count);
    }

    if (show_symbols || getenv("HACK_DEBUG")) {
        print_symbol_table(&table);
    }

    if (verbose) {
        printf("\nAssembly successful: %s\n", output_name);
    }

    // 清理
    fclose(input);
    fclose(output);

    return 0;
}
