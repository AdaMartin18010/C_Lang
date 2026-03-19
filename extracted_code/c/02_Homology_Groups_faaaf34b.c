/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\09_Linking_Algebraic_Topology\02_Homology_Groups.md
 * Line: 604
 * Language: c
 * Block ID: faaaf34b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 1-链的方向很重要！
OneChain ref_forward = { .source = &A, .target = &B };
OneChain ref_backward = { .source = &B, .target = &A };

// 在边缘算子中
// ∂₁(A→B) = B - A
// ∂₁(B→A) = A - B = -(B - A)
// 所以 ∂₁(B→A) = -∂₁(A→B)
