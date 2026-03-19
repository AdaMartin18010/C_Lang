/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\12_Self_Modifying_Code\02_Tracing_JIT.md
 * Line: 682
 * Language: c
 * Block ID: f04e9bb6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简单的trace统计
struct {
    uint64_t trace_enter_count;
    uint64_t guard_fail_count;
    double total_execution_time;
} trace_stats;

void dump_trace_stats(void) {
    printf("Trace进入次数: %llu\n", trace_stats.trace_enter_count);
    printf("Guard失败次数: %llu\n", trace_stats.guard_fail_count);
    printf("平均执行时间: %.3f ms\n",
           trace_stats.total_execution_time / trace_stats.trace_enter_count);
}
