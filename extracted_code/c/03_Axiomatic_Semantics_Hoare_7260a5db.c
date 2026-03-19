/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\00_Core_Semantics_Foundations\03_Axiomatic_Semantics_Hoare.md
 * Line: 136
 * Language: c
 * Block ID: 7260a5db
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

{ n >= 0 }
sum = 0;
i = 0;
while (i < n) {
    sum = sum + a[i];
    i = i + 1;
}
{ sum == Σ(j=0 to n-1) a[j] }
