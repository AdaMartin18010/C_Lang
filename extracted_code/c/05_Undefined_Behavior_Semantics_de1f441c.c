/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\00_Core_Semantics_Foundations\05_Undefined_Behavior_Semantics.md
 * Line: 490
 * Language: c
 * Block ID: de1f441c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void add(int* a, int* b, int n) {
    for (int i = 0; i < n; i++)
        a[i] += b[i];
}
