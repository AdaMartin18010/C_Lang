/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\README.md
 * Line: 50
 * Language: c
 * Block ID: c7db0638
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
