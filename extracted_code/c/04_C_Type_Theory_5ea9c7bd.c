/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\00_Core_Semantics_Foundations\04_C_Type_Theory.md
 * Line: 325
 * Language: c
 * Block ID: 5ea9c7bd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

struct A { int x; };
struct B { int x; };
struct A a; struct B b;
a = b;  // 错误：类型不兼容
