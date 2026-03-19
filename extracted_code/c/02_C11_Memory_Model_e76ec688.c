/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\01_Game_Semantics\02_C11_Memory_Model.md
 * Line: 609
 * Language: c
 * Block ID: e76ec688
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误：未初始化或错误初始化
atomic_int x;  // 未初始化！
atomic_int y = 5;  // 非原子初始化

// ✅ 正确初始化
atomic_int a = ATOMIC_VAR_INIT(5);  // C11方式
atomic_int b;
atomic_init(&b, 5);  // 显式初始化

// 动态分配
_Atomic int *p = malloc(sizeof(_Atomic int));
atomic_init(p, 0);
