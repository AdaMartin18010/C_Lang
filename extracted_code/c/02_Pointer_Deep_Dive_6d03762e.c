/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\02_Pointer_Deep_Dive.md
 * Line: 201
 * Language: c
 * Block ID: 6d03762e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void *vp = malloc(sizeof(int));
// *vp = 42;        // 错误！不能直接解引用
*(int*)vp = 42;     // 正确：先转换类型
int *ip = vp;       // 隐式转换
