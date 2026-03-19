/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\00_Core_Semantics_Foundations\03_Axiomatic_Semantics_Hoare.md
 * Line: 255
 * Language: c
 * Block ID: 607453a2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@ requires \valid(a + (0..n-1));
    requires n >= 0;
    assigns \nothing;
    ensures \result == (\sum(0, n-1, \lambda integer i; a[i]));
*/
int array_sum(int *a, int n) {
    int sum = 0;
    /*@ loop invariant 0 <= i <= n;
        loop invariant sum == \sum(0, i-1, \lambda integer k; a[k]);
        loop assigns i, sum;
        loop variant n - i;
    */
    for (int i = 0; i < n; i++) {
        sum += a[i];
    }
    return sum;
}
