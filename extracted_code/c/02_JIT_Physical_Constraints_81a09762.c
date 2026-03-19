/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\13_Self_Modifying_Code\02_JIT_Physical_Constraints.md
 * Line: 678
 * Language: c
 * Block ID: 81a09762
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 代码缓存占用物理内存，需要有效管理：
 *
 * 1. 空间约束：代码缓存不能无限增长
 * 2. 局部性：保留热代码，淘汰冷代码
 * 3. 碎片化：长期分配导致碎片
 */

// 基于成本的代码GC
typedef struct {
    CodeBlock *block;
    double reuse_probability;   // 预测再次使用概率
    double reconstruction_cost; // 重新编译成本
    size_t memory_size;
} GCEntry;

double calculate_gc_priority(GCEntry *entry) {
    // 优先级 = 内存占用 / (重用概率 × 重建成本)
    // 高优先级 = 更可能被GC

    return entry->memory_size /
           (entry->reuse_probability * entry->reconstruction_cost + 0.001);
}

void code_gc_with_constraints(CodeCache *cache, size_t target_size) {
    // 收集所有条目
    GArray *entries = g_array_new(FALSE, FALSE, sizeof(GCEntry));

    for (GList *l = cache->code_blocks; l != NULL; l = l->next) {
        CodeBlock *block = l->data;

        GCEntry entry = {
            .block = block,
            .reuse_probability = predict_reuse(block),
            .reconstruction_cost = estimate_recompile_cost(block),
            .memory_size = block->size
        };

        g_array_append_val(entries, entry);
    }

    // 按优先级排序（高优先级先删除）
    g_array_sort(entries, compare_gc_priority);

    // 删除直到达到目标大小
    size_t current_size = cache->used;
    for (int i = 0; i < entries->len && current_size > target_size; i++) {
        GCEntry *entry = &g_array_index(entries, GCEntry, i);

        // 删除代码块
        invalidate_code_block(entry->block);
        free_code_block(entry->block);

        current_size -= entry->memory_size;
    }

    // 压缩碎片
    if (cache->fragmentation_ratio > 0.3) {
        compact_code_cache(cache);
    }
}
