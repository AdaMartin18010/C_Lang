/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\01_Formal_Semantics\05_Axiomatic_Semantics_Detailed.md
 * Line: 439
 * Language: c
 * Block ID: e27bffba
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

{n ≥ 0}
i := 0;
s := 0;
while i < n do
    s := s + a[i];
    i := i + 1
{s = Σ_{k=0}^{n-1} a[k]}
