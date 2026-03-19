/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\00_Core_Semantics_Foundations\05_Undefined_Behavior_Semantics.md
 * Line: 178
 * Language: c
 * Block ID: 54424a9c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int *p = malloc(sizeof(int));
free(p);
*p = 42;  // UB！（use-after-free）
