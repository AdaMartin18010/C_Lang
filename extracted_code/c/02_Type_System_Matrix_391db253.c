/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\02_Type_System_Matrix.md
 * Line: 356
 * Language: c
 * Block ID: 391db253
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 有符号/无符号混合运算
unsigned int u = 10;
int s = -1;
if (s < u) {  // s 被转换为 unsigned int
    // 永远不会执行！因为 -1 变成 UINT_MAX
}

// 浮点精度丢失
float f = 16777216.0f;  // 2^24
f = f + 1;  // f 不变！精度不足以表示
