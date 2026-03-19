/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\02_Data_Type_System.md
 * Line: 665
 * Language: c
 * Block ID: 1fa3ea52
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

bool are_compatible_types(type_t t1, type_t t2) {
    if (t1.category != t2.category) return false;

    switch (t1.category) {
        case BASIC:
            return t1.basic == t2.basic;
        case POINTER:
            return are_compatible_types(*t1.pointee, *t2.pointee);
        case ARRAY:
            return t1.size == t2.size &&
                   are_compatible_types(t1.element, t2.element);
        case STRUCT:
        case UNION:
            return t1.tag == t2.tag;  // 同一结构体/联合体类型
        case FUNCTION:
            return are_compatible_types(t1.return_type, t2.return_type) &&
                   t1.param_count == t2.param_count &&
                   all(are_compatible_types(t1.params[i], t2.params[i]));
        default:
            return false;
    }
}
