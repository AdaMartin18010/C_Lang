/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\00_Core_Semantics_Foundations\04_C_Type_Theory.md
 * Line: 371
 * Language: c
 * Block ID: f1694c08
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

float f = 3.14f;
int* pi = (int*)&f;  // 违反严格别名
int i = *pi;         // UB！
