/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 302
 * Language: c
 * Block ID: af050249
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void process_data(int n) {
    double vla[n];

    size_t count = _Countof(vla);  // 在运行时计算，返回n的值

    for (size_t i = 0; i < count; i++) {
        vla[i] = 0.0;
    }
}
