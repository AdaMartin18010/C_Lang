/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\01_Compilation_Functor.md
 * Line: 365
 * Language: c
 * Block ID: 9511930c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void f(int *p, int *q) {
    *p = 1;
    *q = 2;
    return *p;  // 编译器假设p!=q，直接返回1
}
