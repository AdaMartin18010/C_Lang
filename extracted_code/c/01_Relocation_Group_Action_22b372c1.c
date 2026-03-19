/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\09_Linking_Algebraic_Topology\01_Relocation_Group_Action.md
 * Line: 183
 * Language: c
 * Block ID: 22b372c1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 计算地址轨道的概念实现
void compute_address_orbit(
    uint64_t base_addr,
    const RelocOperation *relocs[],
    size_t n_relocs,
    uint64_t *orbit_out,
    size_t *orbit_size
) {
    // 轨道包含所有重定位操作作用后的结果
    HashSet seen = hashset_create();
    Queue to_process = queue_create();

    queue_push(to_process, base_addr);

    while (!queue_empty(to_process)) {
        uint64_t current = queue_pop(to_process);

        if (hashset_contains(seen, current)) continue;
        hashset_add(seen, current);
        orbit_out[(*orbit_size)++] = current;

        // 应用所有群元素（重定位操作）
        for (size_t i = 0; i < n_relocs; i++) {
            uint64_t next = relocs[i]->apply(current, 0);
            if (!hashset_contains(seen, next)) {
                queue_push(to_process, next);
            }
        }
    }
}
