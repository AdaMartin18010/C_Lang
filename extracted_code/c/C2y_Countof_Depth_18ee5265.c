/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 425
 * Language: c
 * Block ID: 18ee5265
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

char buffer[256];

// 安全地获取字符串
strncpy(buffer, source, _Countof(buffer) - 1);
buffer[_Countof(buffer) - 1] = '\0';
