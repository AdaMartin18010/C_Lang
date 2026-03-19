/*
 * Auto-generated from: 07_Modern_Toolchain\12_Modern_C_Standards\README.md
 * Line: 532
 * Language: c
 * Block ID: 0f6fa728
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 迁移检查清单：
 * 1. 确保编译器支持 -std=c99 或 -std=gnu99
 * 2. 替换 // 注释（如果之前用宏模拟）
 * 3. 利用 <stdint.h> 替换自定义类型定义
 * 4. 使用内联函数替换宏函数
 * 5. 使用指定初始化器简化结构初始化
 */

/* 之前 */
typedef unsigned int uint32_t;
#define INLINE

/* 之后 */
#include <stdint.h>
#define INLINE inline
