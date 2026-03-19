/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Multilevel_Linux.md
 * Line: 793
 * Language: c
 * Block ID: 809b8b50
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 升级（取址）
Level N     →  Level N+1
T          →  T*
&value     →  pointer to value

// 降级（解引用）
Level N     →  Level N-1
T*         →  T
*pointer   →  pointed value

// 应用示例
int x;              // Level 0
int *p = &x;        // Level 1 = &Level 0
int **pp = &p;      // Level 2 = &Level 1

**pp = 42;          // Level 2 → Level 0 (两次解引用)
*pp = &x;           // Level 2 → Level 1 (一次解引用)
