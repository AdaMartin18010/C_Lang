/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 1982
 * Language: c
 * Block ID: 7a03dd71
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 对于数组 arr[N]
sizeof(arr) == _Countof(arr) * sizeof(arr[0])

// 计算总元素数（多维）
total = _Countof(arr) * _Countof(arr[0]) * ... * _Countof(arr[0]...[0])
