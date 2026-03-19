/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\01_Compilation_Functor.md
 * Line: 256
 * Language: c
 * Block ID: a1af2074
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 优化前
int expr = (x * 4) + (x * 2);

// 优化后 (GCC -O2)
int expr = x * 6;
