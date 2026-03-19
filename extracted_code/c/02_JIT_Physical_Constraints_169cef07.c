/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\13_Self_Modifying_Code\02_JIT_Physical_Constraints.md
 * Line: 776
 * Language: c
 * Block ID: 169cef07
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：每次修改都同步整个代码缓存
void inefficient_sync(void) {
    for (int i = 0; i < num_patches; i++) {
        apply_patch(i);
        flush_entire_icache();  // ❌ 过度同步
    }
}

// 正确：批量修改后统一同步
void efficient_sync(void) {
    void *min_addr = (void*)UINTPTR_MAX;
    void *max_addr = NULL;

    for (int i = 0; i < num_patches; i++) {
        apply_patch(i);
        min_addr = min(min_addr, patch_addr(i));
        max_addr = max(max_addr, patch_addr(i) + patch_size(i));
    }

    flush_icache_range(min_addr, max_addr);  // ✅ 单次同步
}
