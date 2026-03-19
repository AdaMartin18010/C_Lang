/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 452
 * Language: c
 * Block ID: 8044bec7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int arr[100];

size_t element_count = _Countof(arr);       // 100
size_t total_bytes = sizeof(arr);            // 400（假设int为4字节）
size_t element_size = sizeof(arr[0]);        // 4

// 验证关系
assert(total_bytes == element_count * element_size);
