/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\01_Game_Semantics\02_C11_Memory_Model.md
 * Line: 92
 * Language: c
 * Block ID: 1b119a68
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 原子操作头文件
#include <stdatomic.h>
#include <threads.h>
#include <stdbool.h>

// 原子类型声明
_Atomic int counter = ATOMIC_VAR_INIT(0);
// 或使用宏
atomic_int shared_var = 0;
