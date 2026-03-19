/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 78
 * Language: c
 * Block ID: 755cb0f0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 需要多次书写数组名和元素类型
sizeof(my_array) / sizeof(my_array[0])
//         ^^^^^^          ^^^^^^^^^^^^
//         重复书写，容易出错
