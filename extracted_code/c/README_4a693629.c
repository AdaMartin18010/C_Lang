/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\README.md
 * Line: 305
 * Language: c
 * Block ID: 4a693629
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
