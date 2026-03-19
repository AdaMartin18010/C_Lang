/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\05_Heap_Priority_Queue.md
 * Line: 193
 * Language: c
 * Block ID: e2ccf098
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 父节点索引
#define PARENT(i) (((i) - 1) / 2)

// 左子节点索引
#define LEFT(i)   (2 * (i) + 1)

// 右子节点索引
#define RIGHT(i)  (2 * (i) + 2)

// 是否为叶子节点
#define IS_LEAF(heap, i) ((i) >= (heap)->size / 2)
