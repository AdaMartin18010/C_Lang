/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\02_Pointer_Deep_Dive.md
 * Line: 415
 * Language: c
 * Block ID: 48fae62a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int x = 42;
int *p = &x;
int **pp = &p;

// 多级解引用
x      → 42
*p     → 42 (通过p访问x)
**pp   → 42 (通过pp访问p, 再访问x)
