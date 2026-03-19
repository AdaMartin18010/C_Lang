/*
 * Auto-generated from: 06_Thinking_Representation\09_Bloom_Taxonomy\03_Understanding_Checks.md
 * Line: 532
 * Language: c
 * Block ID: a6288cf4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

T5: 绘制动态内存分配图
    int *p1 = malloc(100);
    int *p2 = calloc(10, sizeof(int));
    int *p3 = realloc(p1, 200);

要求:
- 显示堆内存区域
- 标注分配的内存块、元数据、空闲块
- 说明realloc可能的两种情况(原地扩展/重新分配)
