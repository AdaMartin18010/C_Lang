/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\README.md
 * Line: 71
 * Language: c
 * Block ID: 8faa5593
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
