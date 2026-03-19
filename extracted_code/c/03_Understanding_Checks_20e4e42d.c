/*
 * Auto-generated from: 06_Thinking_Representation\09_Bloom_Taxonomy\03_Understanding_Checks.md
 * Line: 985
 * Language: c
 * Block ID: 20e4e42d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

S19: 以下代码在64位系统上可能有什么问题？
     int *p = malloc(1000000 * sizeof(int));

答案: 乘法溢出。1000000 * sizeof(int)可能溢出32位。
     应使用: malloc(1000000 * sizeof *p);
     或使用calloc: calloc(1000000, sizeof(int));
