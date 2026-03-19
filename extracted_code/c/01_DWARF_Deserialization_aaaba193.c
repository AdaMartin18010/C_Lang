/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\01_DWARF_Deserialization.md
 * Line: 212
 * Language: c
 * Block ID: aaaba193
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// DIE属性形式
typedef enum {
    DW_FORM_addr = 0x01,        // 地址
    DW_FORM_block2 = 0x03,      // 2字节长度块
    DW_FORM_block4 = 0x04,      // 4字节长度块
    DW_FORM_data2 = 0x05,       // 2字节常量
    DW_FORM_data4 = 0x06,       // 4字节常量
    DW_FORM_data8 = 0x07,       // 8字节常量
    DW_FORM_string = 0x08,      // 空终止字符串
    DW_FORM_block = 0x09,       // ULEB128长度块
    DW_FORM_block1 = 0x0a,      // 1字节长度块
    DW_FORM_data1 = 0x0b,       // 1字节常量
    DW_FORM_flag = 0x0c,        // 标志（1字节）
    DW_FORM_sdata = 0x0d,       // SLEB128常量
    DW_FORM_strp = 0x0e,        // 字符串表偏移
    DW_FORM_udata = 0x0f,       // ULEB128常量
    DW_FORM_ref_addr = 0x10,    // 引用（跨编译单元）
    DW_FORM_ref1 = 0x11,        // 1字节偏移引用
    DW_FORM_ref2 = 0x12,        // 2字节偏移引用
    DW_FORM_ref4 = 0x13,        // 4字节偏移引用
    DW_FORM_ref8 = 0x14,        // 8字节偏移引用
    DW_FORM_ref_udata = 0x15,   // ULEB128偏移引用
    DW_FORM_indirect = 0x16,    // 间接形式
    // DWARFv4+
    DW_FORM_sec_offset = 0x17,  // 节偏移
    DW_FORM_exprloc = 0x18,     // DWARF表达式
    DW_FORM_flag_present = 0x19,// 隐式标志存在
    DW_FORM_ref_sig8 = 0x20,    // 类型签名引用
    // DWARFv5
    DW_FORM_strx = 0x1a,        // 字符串索引
    DW_FORM_addrx = 0x1b,       // 地址索引
    DW_FORM_ref_sup4 = 0x1c,    // 补充节引用
    DW_FORM_strp_sup = 0x1d,    // 补充字符串
    DW_FORM_ref_sup8 = 0x1e,
    DW_FORM_addrx1 = 0x21,      // 1字节地址索引
    DW_FORM_addrx2 = 0x22,      // 2字节地址索引
    DW_FORM_addrx3 = 0x23,      // 3字节地址索引
    DW_FORM_addrx4 = 0x24,      // 4字节地址索引
} Dwarf_Form;

// 属性值
typedef struct {
    Dwarf_Form form;
    union {
        uint64_t uval;
        int64_t sval;
        const char *string;
        uint8_t *block;
        size_t block_len;
        bool flag;
        uint64_t addr;
        uint64_t ref;       // DIE引用
        uint64_t sec_offset;
    };
} Dwarf_AttributeValue;

// DIE结构
typedef struct Dwarf_Die {
    uint64_t offset;                    // 在.debug_info中的偏移
    uint32_t abbrev_code;               // 缩略码
    uint16_t tag;                       // 标签（DW_TAG_*）
    bool has_children;                  // 是否有子DIE

    // 属性
    Dwarf_AttributeValue *attributes;
    uint16_t *attr_names;
    int num_attributes;

    // 树结构
    struct Dwarf_Die *parent;
    struct Dwarf_Die *child;
    struct Dwarf_Die *sibling;
} Dwarf_Die;
