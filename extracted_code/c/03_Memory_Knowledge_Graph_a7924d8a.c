/*
 * Auto-generated from: 06_Thinking_Representation\07_Knowledge_Graph\03_Memory_Knowledge_Graph.md
 * Line: 205
 * Language: c
 * Block ID: a7924d8a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// malloc - 分配指定字节数的未初始化内存
void *p1 = malloc(100);           // 100 bytes, 未初始化

// calloc - 分配 n * size 字节并清零
void *p2 = calloc(10, sizeof(int)); // 10个int, 全部置0

// realloc - 重新调整已分配内存大小
void *p3 = realloc(p1, 200);       // 扩展到200字节

// aligned_alloc - 按指定对齐方式分配
void *p4 = aligned_alloc(64, 256); // 64字节对齐, 256字节

// free - 释放内存
free(p2);
free(p3);
free(p4);
