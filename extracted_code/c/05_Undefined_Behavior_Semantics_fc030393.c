/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\00_Core_Semantics_Foundations\05_Undefined_Behavior_Semantics.md
 * Line: 503
 * Language: c
 * Block ID: fc030393
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int x;
if (some_condition)
    x = 5;
// 未检查some_condition
return x;  // UB如果some_condition为假
