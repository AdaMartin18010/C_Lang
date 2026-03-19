/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 1530
 * Language: c
 * Block ID: b061d9b2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void process_vla(int n) {
    int vla[n];  // VLA

    // 编译器需要存储n的值，以便后续使用_Countof
    size_t count = _Countof(vla);  // 运行时读取n的值
}
