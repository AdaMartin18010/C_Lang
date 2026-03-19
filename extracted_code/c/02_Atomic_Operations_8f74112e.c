/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\02_Atomic_Operations.md
 * Line: 389
 * Language: c
 * Block ID: 8f74112e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：非原子读取原子变量
atomic_int flag = 0;
int x = flag;  // 未定义行为！

// 正确
int x = atomic_load(&flag);
