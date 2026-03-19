/*
 * Auto-generated from: 01_Core_Knowledge_System\03_Construction_Layer\03_Modularization_Linking.md
 * Line: 279
 * Language: c
 * Block ID: 6f48bc9a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误：在头文件中定义变量
// config.h
int debug_level = 0;  // 每个包含此文件的.c都会有一个定义！

// ✅ 正确：头文件只声明，一个.c文件定义
// config.h
extern int debug_level;  // 声明

// config.c
int debug_level = 0;  // 定义（只有一个）
