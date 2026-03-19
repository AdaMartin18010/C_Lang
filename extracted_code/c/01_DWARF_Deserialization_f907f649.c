/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\01_DWARF_Deserialization.md
 * Line: 1136
 * Language: c
 * Block ID: f907f649
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：混淆DW_FORM_string和DW_FORM_strp
void wrong_string_handling(Dwarf_AttributeValue *attr) {
    printf("%s\n", (char *)attr->uval);  // ❌ DW_FORM_strp是偏移量！
}

// 正确：根据form类型正确获取字符串
const char *correct_get_string(Dwarf_AttributeValue *attr,
                                const uint8_t *debug_str,
                                size_t debug_str_size) {
    switch (attr->form) {
        case DW_FORM_string:
            // 内联字符串，直接指向数据
            return attr->string;

        case DW_FORM_strp:
            // 字符串表偏移，需要验证边界
            if (attr->uval < debug_str_size) {
                return (const char *)(debug_str + attr->uval);
            }
            return "<invalid string offset>";

        case DW_FORM_strx:
        case DW_FORM_strx1:
        case DW_FORM_strx2:
        case DW_FORM_strx3:
        case DW_FORM_strx4:
            // DWARFv5字符串索引，需要查字符串偏移表
            return resolve_strx(attr->uval);

        default:
            return "<not a string>";
    }
}
