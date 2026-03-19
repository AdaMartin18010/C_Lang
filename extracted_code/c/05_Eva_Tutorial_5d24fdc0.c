/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Eva_Tutorial.md
 * Line: 716
 * Language: c
 * Block ID: 5d24fdc0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// tutorial9_pointer.c
#include <stdlib.h>

// 无效指针访问
void risky_dereference(int* p) {
    *p = 42;  // 如果p为NULL,崩溃!
}

// 安全指针访问
/*@
  @requires p != \null;
  @requires \valid(p);
  @assigns *p;
  @ensures *p == 42;
*/
void safe_dereference(int* p) {
    *p = 42;
}

// NULL检查
void checked_dereference(int* p) {
    if (p != NULL) {
        *p = 42;
    }
}
