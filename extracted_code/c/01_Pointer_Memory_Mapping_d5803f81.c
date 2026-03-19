/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\01_Pointer_Memory_Mapping.md
 * Line: 151
 * Language: c
 * Block ID: d5803f81
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 指针语义层
int *p = malloc(sizeof(int));  // 指针p获得地址
*p = 42;                        // 通过指针访问
free(p);                        // 通过指针释放

// 内存语义层（等价视角）
// 1. 在堆上分配 sizeof(int) 字节的内存块
// 2. 内存块获得初始地址，通过p引用
// 3. 在该内存位置写入值42
// 4. 释放该内存块，p成为悬挂指针
