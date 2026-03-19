/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\09_Linking_Algebraic_Topology\01_Relocation_Group_Action.md
 * Line: 290
 * Language: c
 * Block ID: 11d41612
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 地址空间的拓扑结构
typedef struct {
    uint64_t start;
    uint64_t end;
} AddressInterval;

// 开区间 (a, b) - 不包含端点
bool in_open_interval(uint64_t x, AddressInterval iv) {
    return x > iv.start && x < iv.end;
}

// 闭区间 [a, b] - 包含端点
bool in_closed_interval(uint64_t x, AddressInterval iv) {
    return x >= iv.start && x <= iv.end;
}

// 连续性定义：链接映射保持邻近关系
// f: X → Y 连续 ⇔ ∀U⊆Y开, f⁻¹(U)⊆X开
typedef struct {
    AddressInterval domain;
    uint64_t (*map)(uint64_t);
} ContinuousMap;

// 验证映射连续性
bool is_continuous(
    const ContinuousMap *f,
    const AddressInterval *open_sets_y[],
    size_t n_open_y,
    const AddressInterval *open_sets_x[],
    size_t n_open_x
) {
    // ∀U⊆Y开, 检查f⁻¹(U)是否开
    for (size_t i = 0; i < n_open_y; i++) {
        AddressInterval preimage = {
            .start = inverse_image(f->map, open_sets_y[i]->start),
            .end = inverse_image(f->map, open_sets_y[i]->end)
        };

        bool is_open = false;
        for (size_t j = 0; j < n_open_x; j++) {
            if (interval_equals(preimage, *open_sets_x[j])) {
                is_open = true;
                break;
            }
        }
        if (!is_open) return false;
    }
    return true;
}
