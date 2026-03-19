/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\README.md
 * Line: 584
 * Language: c
 * Block ID: 0aabfd66
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
