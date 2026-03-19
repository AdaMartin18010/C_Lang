/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\08_Linking_Loading_Topology\README.md
 * Line: 85
 * Language: c
 * Block ID: a0d5dad2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 原始代码
#define MAX_SIZE 1024
#define SQUARE(x) ((x) * (x))

int buffer[MAX_SIZE];
int result = SQUARE(5 + 3);

// 预处理后
int buffer[1024];
int result = ((5 + 3) * (5 + 3));
