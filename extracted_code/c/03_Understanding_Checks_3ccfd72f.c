/*
 * Auto-generated from: 06_Thinking_Representation\09_Bloom_Taxonomy\03_Understanding_Checks.md
 * Line: 495
 * Language: c
 * Block ID: 3ccfd72f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

T2: 绘制结构体的内存布局
    struct Example {
        char a;
        int b;
        short c;
    };

要求:
- 考虑默认对齐
- 标出填充字节位置
- 计算sizeof(struct Example)
- 画出重排后的优化布局
