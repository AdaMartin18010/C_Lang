/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\01_C11_Memory_Model.md
 * Line: 723
 * Language: c
 * Block ID: 32848cd6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 决策树 */
void memory_order_guide(void) {
    /*
     * 问题1: 是否只需要原子性，不需要同步?
     *   是 -> memory_order_relaxed
     *
     * 问题2: 是否发布数据给另一个线程?
     *   是 -> memory_order_release (写) + memory_order_acquire (读)
     *
     * 问题3: 是否需要全局顺序?
     *   是 -> memory_order_seq_cst
     *
     * 默认选择: seq_cst (正确性优先)
     * 性能优化: 根据场景降级到 acquire/release/relaxed
     */
}

/* 最佳实践 */
/* 1. 默认使用 seq_cst，性能瓶颈时再优化 */
/* 2. 明确文档注释内存序选择的原因 */
/* 3. 配对使用 release-acquire */
/* 4. 谨慎使用 relaxed */
