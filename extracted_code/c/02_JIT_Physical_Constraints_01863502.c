/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\13_Self_Modifying_Code\02_JIT_Physical_Constraints.md
 * Line: 84
 * Language: c
 * Block ID: 01863502
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * JIT编译产生的代码面临独特的内存局部性挑战：
 *
 * 1. 代码生成时间局部性：编译时访问的数据（IR、符号表）
 *    与运行时执行的代码分离
 *
 * 2. 代码空间局部性：动态生成的代码块可能分散在内存中
 *    导致指令缓存未命中
 *
 * 3. 数据-代码分离：JIT生成代码后，数据访问模式改变
 */

// 代码缓存布局优化
typedef struct {
    // 热代码区域（频繁执行）
    void *hot_code_start;
    size_t hot_code_size;

    // 冷代码区域（较少执行）
    void *cold_code_start;
    size_t cold_code_size;

    // 元数据区域（非执行）
    void *metadata_start;
    size_t metadata_size;
} CodeCacheLayout;

// 基于执行频率的代码重排
void optimize_code_layout(CodeCache *cache) {
    // 分析执行频率
    GHashTable *freq_map = profile_execution_frequency(cache);

    // 收集所有代码块
    GList *blocks = g_hash_table_get_values(cache->blocks);

    // 按执行频率排序
    blocks = g_list_sort(blocks, compare_by_frequency);

    // 重新布局：热代码在前，紧密排列
    void *current = cache->base_addr;
    for (GList *l = blocks; l != NULL; l = l->next) {
        CodeBlock *block = l->data;

        if (block->exec_count > HOT_THRESHOLD) {
            // 热代码：64字节对齐（适配缓存行）
            current = (void*)(((uintptr_t)current + 63) & ~63);

            // 复制到新位置
            memcpy(current, block->start, block->size);

            // 更新跳转目标
            patch_references(block, current);

            block->start = current;
            current += block->size;
        }
    }

    // 刷新指令缓存
    jit_flush_icache(cache->base_addr, cache->used);
}

// 缓存行对齐的代码分配
void* alloc_cache_aligned(CodeCache *cache, size_t size, int exec_count) {
    size_t alignment = (exec_count > HOT_THRESHOLD) ? 64 : 16;

    uintptr_t addr = (uintptr_t)cache->base_addr + cache->used;
    addr = (addr + alignment - 1) & ~(alignment - 1);

    size_t padding = addr - ((uintptr_t)cache->base_addr + cache->used);
    cache->used += padding + size;

    return (void*)addr;
}
