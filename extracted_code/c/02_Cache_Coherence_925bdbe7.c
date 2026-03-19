/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\02_Cache_Coherence.md
 * Line: 816
 * Language: c
 * Block ID: 925bdbe7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: DMA操作前后未刷新缓存
void dma_transfer(void *buffer, size_t size) {
    // 直接启动DMA，缓存数据可能未写入内存
    start_dma(buffer, size);
}

// ✅ 正确: DMA前clean，DMA后invalidate
void dma_transfer_fixed(void *buffer, size_t size) {
    // DMA从内存读: clean dcache
    dma_sync_single_for_device(dev, addr, size, DMA_TO_DEVICE);

    start_dma(buffer, size);
    wait_dma_complete();

    // DMA写入内存: invalidate dcache
    dma_sync_single_for_cpu(dev, addr, size, DMA_FROM_DEVICE);
}
