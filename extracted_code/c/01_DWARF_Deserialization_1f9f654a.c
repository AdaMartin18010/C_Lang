/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\01_DWARF_Deserialization.md
 * Line: 1100
 * Language: c
 * Block ID: 1f9f654a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：将相对引用当作绝对引用
uint64_t wrong_resolve_ref(uint64_t ref_offset, uint64_t cu_start) {
    return ref_offset;  // ❌ 忽略了引用类型！
}

// 正确：根据form类型处理引用
uint64_t correct_resolve_ref(Dwarf_AttributeValue *attr,
                              uint64_t die_offset,
                              uint64_t cu_start,
                              const Dwarf_Context *ctx) {
    switch (attr->form) {
        case DW_FORM_ref1:
        case DW_FORM_ref2:
        case DW_FORM_ref4:
        case DW_FORM_ref8:
        case DW_FORM_ref_udata:
            // 相对当前CU的偏移
            return cu_start + attr->ref;

        case DW_FORM_ref_addr:
            // 绝对偏移（跨CU引用）
            return attr->ref;

        case DW_FORM_ref_sig8:
            // 类型签名引用，需要查找类型表
            return lookup_type_by_signature(ctx, attr->ref);

        default:
            return 0;
    }
}
