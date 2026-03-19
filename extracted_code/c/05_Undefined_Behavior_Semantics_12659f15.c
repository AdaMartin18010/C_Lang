/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\00_Core_Semantics_Foundations\05_Undefined_Behavior_Semantics.md
 * Line: 207
 * Language: c
 * Block ID: 12659f15
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

float f = 3.14f;
int* pi = (int*)&f;
int i = *pi;         // UB: float* 通过 int* 访问
