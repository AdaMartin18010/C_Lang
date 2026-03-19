/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\09_Linking_Algebraic_Topology\01_Relocation_Group_Action.md
 * Line: 588
 * Language: c
 * Block ID: 87b39737
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 警告：重定位操作一般不满足交换律
// r₁ ∘ r₂ ≠ r₂ ∘ r₁

void dangerous_reordering(RelocOperation *r1, RelocOperation *r2,
                          uint64_t addr) {
    uint64_t result1 = r1->apply(r2->apply(addr, 0), 0);
    uint64_t result2 = r2->apply(r1->apply(addr, 0), 0);

    // 如果r1和r2涉及同一地址，结果可能不同！
    assert(result1 == result2);  // 可能失败！
}
