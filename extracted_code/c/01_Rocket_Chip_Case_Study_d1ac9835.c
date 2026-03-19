/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\01_Rocket_Chip_Case_Study.md
 * Line: 979
 * Language: c
 * Block ID: d1ac9835
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：DMA传输后未刷新缓存
void dma_transfer(void* src, void* dst, size_t size) {
    dma_start(src, dst, size);
    dma_wait();
    // 缺少缓存同步！
}

// 正确：添加缓存同步
void dma_transfer_fixed(void* src, void* dst, size_t size) {
    // 1. 清理源地址缓存 (写回)
    cache_clean_range(src, size);

    // 2. 启动DMA
    dma_start(src, dst, size);
    dma_wait();

    // 3. 使目标地址缓存失效
    cache_invalidate_range(dst, size);
}
