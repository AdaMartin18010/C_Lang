/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\01_C11_Memory_Model.md
 * Line: 773
 * Language: c
 * Block ID: 7eae4603
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 选择指南 */
void lock_vs_lockfree(void) {
    /*
     * 使用锁当:
     * - 竞争不激烈
     * - 需要复杂的数据结构不变式
     * - 代码可读性优先
     * - 需要条件变量等阻塞语义
     *
     * 使用无锁当:
     * - 竞争激烈（高并发）
     * - 需要确定性的延迟（无阻塞）
     * - 实现简单的数据结构（栈、队列、计数器）
     * - 专家级性能优化
     *
     * 注意: 无锁代码更难编写和验证，谨慎使用!
     */
}
