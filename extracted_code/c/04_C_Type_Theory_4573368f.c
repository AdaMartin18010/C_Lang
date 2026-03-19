/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\00_Core_Semantics_Foundations\04_C_Type_Theory.md
 * Line: 379
 * Language: c
 * Block ID: 4573368f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

union FloatInt { float f; int i; };
union FloatInt fi = { .f = 3.14f };
int i = fi.i;  // OK
