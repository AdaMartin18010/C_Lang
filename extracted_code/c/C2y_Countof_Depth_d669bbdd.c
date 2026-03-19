/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 1635
 * Language: c
 * Block ID: d669bbdd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23现代化
for (int i = 0; i < _Countof(arr); i++) {
    process(arr[i]);
}

// 更好的方式：使用size_t
for (size_t i = 0; i < _Countof(arr); i++) {
    process(arr[i]);
}
