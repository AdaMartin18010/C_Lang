/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\00_Core_Semantics_Foundations\03_Axiomatic_Semantics_Hoare.md
 * Line: 97
 * Language: c
 * Block ID: a6d6a6f6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

{ n >= 0 }
i = 0;
f = 1;
while (i < n) {
    i = i + 1;
    f = f * i;
}
{ f == n! }
