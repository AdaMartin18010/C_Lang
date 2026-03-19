/*
 * Auto-generated from: 06_Thinking_Representation\07_Knowledge_Graph\02_Pointer_Knowledge_Graph.md
 * Line: 174
 * Language: c
 * Block ID: 071ad5af
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 数组与指针等价关系
a[i]       == *(a + i)      // 数组索引
&a[i]      == a + i         // 取数组元素地址
*(p + i)   == p[i]          // 指针索引
p++        // 移动 sizeof(*p) 字节
