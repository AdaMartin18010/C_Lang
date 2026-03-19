/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\00_Core_Semantics_Foundations\05_Undefined_Behavior_Semantics.md
 * Line: 309
 * Language: c
 * Block ID: 3902d005
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void f(int* p) {
    int x = *p;      // 如果p为null，这是UB
    if (p == NULL)   // 编译器可假设永不为真
        return;      // 可删除此分支
    // ...
}
