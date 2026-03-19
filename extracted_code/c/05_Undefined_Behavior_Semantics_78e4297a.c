/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\00_Core_Semantics_Foundations\05_Undefined_Behavior_Semantics.md
 * Line: 215
 * Language: c
 * Block ID: 78e4297a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

union { float f; int i; } u;
u.f = 3.14f;
int i = u.i;         // OK: 通过union访问
