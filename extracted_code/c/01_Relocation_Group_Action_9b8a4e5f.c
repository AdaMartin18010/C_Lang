/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\09_Linking_Algebraic_Topology\01_Relocation_Group_Action.md
 * Line: 226
 * Language: c
 * Block ID: 9b8a4e5f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 稳定子群的计算
// Stab(a) = {r ∈ R | φ(r, a) = a}
void compute_stabilizer(
    uint64_t address,
    const RelocOperation *relocs[],
    size_t n_relocs,
    RelocOperation **stabilizer_out,
    size_t *stabilizer_size
) {
    for (size_t i = 0; i < n_relocs; i++) {
        uint64_t result = relocs[i]->apply(address, 0);
        // 群元素保持地址不变 ⇔ 属于稳定子群
        if (result == address) {
            stabilizer_out[(*stabilizer_size)++] =
                (RelocOperation *)relocs[i];
        }
    }
}
