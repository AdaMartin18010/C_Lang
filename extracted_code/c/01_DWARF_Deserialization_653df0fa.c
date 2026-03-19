/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\01_DWARF_Deserialization.md
 * Line: 468
 * Language: c
 * Block ID: 653df0fa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 行号程序寄存器
typedef struct {
    uint64_t address;       // 地址
    int file;               // 文件索引（1-based）
    int line;               // 行号（1-based）
    int column;             // 列号（1-based）
    bool is_stmt;           // 是否是语句开始
    bool basic_block;       // 是否是基本块开始
    bool end_sequence;      // 行号序列结束
    bool prologue_end;      // 函数序言结束
    bool epilogue_begin;    // 函数尾声开始
    uint64_t isa;           // 指令集架构
    uint64_t discriminator; // 鉴别器
} LineNumberState;

// 标准操作码
typedef enum {
    DW_LNS_copy = 1,            // 复制当前状态到矩阵，恢复is_stmt
    DW_LNS_advance_pc,          // 地址 += delta * min_inst_length
    DW_LNS_advance_line,        // 行号 += delta
    DW_LNS_set_file,            // 设置文件
    DW_LNS_set_column,          // 设置列
    DW_LNS_negate_stmt,         // 翻转is_stmt
    DW_LNS_set_basic_block,     // 设置basic_block
    DW_LNS_const_add_pc,        // 地址 += (255 - opcode_base) / line_range
    DW_LNS_fixed_advance_pc,    // 地址 += operand
    DW_LNS_set_prologue_end,    // 设置prologue_end
    DW_LNS_set_epilogue_begin,  // 设置epilogue_begin
    DW_LNS_set_isa,             // 设置isa
} Dwarf_LineNumberOpcode;

// 扩展操作码
typedef enum {
    DW_LNE_end_sequence = 1,    // 结束序列
    DW_LNE_set_address,         // 设置绝对地址
    DW_LNE_define_file,         // 定义文件
    DW_LNE_set_discriminator,   // 设置鉴别器
    // DWARFv5
    DW_LNE_lo_user = 0x80,
    DW_LNE_hi_user = 0xff,
} Dwarf_LineNumberExtendedOpcode;

// 行号表头
typedef struct {
    uint32_t unit_length;
    uint16_t version;
    uint8_t address_size;       // DWARFv5
    uint8_t segment_selector_size; // DWARFv5
    uint32_t header_length;
    uint8_t minimum_instruction_length;
    uint8_t maximum_operations_per_instruction; // DWARFv4+
    uint8_t default_is_stmt;
    int8_t line_base;
    uint8_t line_range;
    uint8_t opcode_base;
    uint8_t *standard_opcode_lengths;

    char **include_directories;
    int num_include_dirs;

    struct {
        char *name;
        uint64_t dir_index;
        uint64_t mtime;
        uint64_t length;
    } *file_names;
    int num_file_names;
} LineNumberProgramHeader;

// 行号矩阵条目
typedef struct {
    uint64_t address;
    int file;
    int line;
    int column;
    bool is_stmt;
    bool basic_block;
    bool end_sequence;
    bool prologue_end;
    bool epilogue_begin;
    uint64_t isa;
    uint64_t discriminator;
} LineNumberEntry;
