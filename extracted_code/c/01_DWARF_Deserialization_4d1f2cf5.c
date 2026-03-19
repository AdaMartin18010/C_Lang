/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\01_DWARF_Deserialization.md
 * Line: 82
 * Language: c
 * Block ID: 4d1f2cf5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 典型的DWARF ELF节
/*
 * .debug_abbrev    - 缩略码表（属性规范）
 * .debug_info      - 主要的调试信息条目(DIE)
 * .debug_line      - 行号信息
 * .debug_str       - 字符串表
 * .debug_loc       - 位置列表
 * .debug_ranges    - 地址范围
 * .debug_frame     - 调用帧信息(CFI)
 * .debug_aranges   - 地址到编译单元的映射
 * .debug_pubnames  - 全局名称查找表
 * .debug_types     - 类型定义（DWARFv4+）
 */

// DWARF节头（通用结构）
typedef struct {
    uint32_t unit_length;       // 单元长度（4字节或12字节扩展）
    uint16_t version;           // DWARF版本
    uint8_t unit_type;          // 单元类型（DWARFv5）
    uint8_t address_size;       // 地址大小
    uint8_t debug_abbrev_offset;// 缩略码表偏移
} Dwarf_CompileUnitHeader;

// 完整DWARF上下文
typedef struct {
    Elf *elf;                           // ELF句柄

    // 各节数据
    Elf_Data *debug_info;
    Elf_Data *debug_abbrev;
    Elf_Data *debug_line;
    Elf_Data *debug_str;
    Elf_Data *debug_loc;
    Elf_Data *debug_ranges;
    Elf_Data *debug_frame;

    // 解析状态
    GHashTable *abbrev_table;           // 缩略码缓存
    GHashTable *string_cache;           // 字符串缓存
} Dwarf_Context;
