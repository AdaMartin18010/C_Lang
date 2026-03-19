/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 1541
 * Language: c
 * Block ID: 40f47712
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void process_vla(int n) {
    // 编译器隐式保存n
    int _vla_size = n;
    int vla[n];

    size_t count = _vla_size;  // 使用保存的值
}
