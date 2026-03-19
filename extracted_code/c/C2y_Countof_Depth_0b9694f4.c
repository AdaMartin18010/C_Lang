/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 1438
 * Language: c
 * Block ID: 0b9694f4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 编译器处理逻辑
void process_countof(Expr *operand) {
    Type *type = operand->type;

    // 确保操作数是数组类型
    if (!is_array_type(type)) {
        error("_Countof requires array type");
    }

    // 提取数组维度
    size_t count = get_array_size(type);

    // 替换为常量
    return create_constant(count);
}
