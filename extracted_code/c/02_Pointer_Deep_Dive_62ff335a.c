/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\02_Pointer_Deep_Dive.md
 * Line: 633
 * Language: c
 * Block ID: 62ff335a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 野指针示例
int *p;           // 未初始化，值不确定
*p = 42;          // 灾难！可能破坏任意内存

// 正确做法
int *p = NULL;    // 初始化为NULL
if (p != NULL) {  // 使用前检查
    *p = 42;
}
