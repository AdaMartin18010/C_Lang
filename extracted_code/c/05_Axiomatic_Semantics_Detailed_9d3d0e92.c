/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\01_Formal_Semantics\05_Axiomatic_Semantics_Detailed.md
 * Line: 486
 * Language: c
 * Block ID: 9d3d0e92
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

{sorted(a) ∧ n ≥ 0 ∧ x ∈ a[0..n-1]}
low := 0;
high := n - 1;
found := false;
while low ≤ high ∧ ¬found do
    mid := (low + high) / 2;
    if a[mid] = x then
        found := true
    else if a[mid] < x then
        low := mid + 1
    else
        high := mid - 1
{found → a[mid] = x}
