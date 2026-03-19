/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Multilevel_Linux.md
 * Line: 178
 * Language: c
 * Block ID: 7a95a5af
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 类型声明规律：*的数量 = 指针级别
int    x;      // Level 0: int
int   *p;      // Level 1: pointer to int
int  **pp;     // Level 2: pointer to pointer to int
int ***ppp;    // Level 3: pointer to pointer to pointer to int

// 类型大小（64位系统）
sizeof(x)    == 4;   // int大小
sizeof(p)    == 8;   // 所有指针同宽
sizeof(pp)   == 8;   // 与级别无关
sizeof(ppp)  == 8;   // 都是地址宽度
