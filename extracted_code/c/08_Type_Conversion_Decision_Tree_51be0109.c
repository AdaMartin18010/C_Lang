/*
 * Auto-generated from: 06_Thinking_Representation\01_Decision_Trees\08_Type_Conversion_Decision_Tree.md
 * Line: 263
 * Language: c
 * Block ID: 51be0109
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 问题代码
unsigned int a = 1;
int b = -2;
if (a + b > 0)  // b被提升为unsigned，结果很大
    printf("positive\n");  // 会执行！

// ✅ 正确做法
if (a > (unsigned int)(-b))  // 显式比较
    printf("positive\n");
