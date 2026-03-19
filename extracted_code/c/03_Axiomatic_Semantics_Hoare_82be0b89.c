/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\00_Core_Semantics_Foundations\03_Axiomatic_Semantics_Hoare.md
 * Line: 165
 * Language: c
 * Block ID: 82be0b89
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@ requires n >= 0;
    decreases n;        // 终止度量
    assigns \nothing;   // 不修改全局状态
    ensures \result == fact(n);
*/
int factorial(int n) {
    if (n == 0) return 1;
    return n * factorial(n - 1);
}
