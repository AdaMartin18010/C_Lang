/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 413
 * Language: c
 * Block ID: 82815441
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用数组引用保持类型信息
void process_array_ref(int (*arr)[10]) {
    // *arr 是 int[10] 类型
    size_t count = _Countof(*arr);  // 返回 10
}
