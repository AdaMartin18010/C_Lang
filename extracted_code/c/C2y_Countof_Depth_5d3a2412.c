/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 1621
 * Language: c
 * Block ID: 5d3a2412
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 传统循环
for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++) {
    process(arr[i]);
}

// 使用宏
for (int i = 0; i < ARRAY_SIZE(arr); i++) {
    process(arr[i]);
}
