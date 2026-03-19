/*
 * Auto-generated from: 11_Machine_Learning_C\07_llama_cpp_Deployment.md
 * Line: 529
 * Language: c
 * Block ID: afffe660
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 计时和内存统计
void log_performance_stats(llama_context* ctx) {
    struct llama_timings timings = llama_get_timings(ctx);

    printf("Performance Stats:\n");
    printf("  Prompt eval time: %.2f ms / %d tokens (%.2f ms/tok)\n",
           timings.t_p_eval_ms,
           timings.n_p_eval,
           timings.t_p_eval_ms / timings.n_p_eval);

    printf("  Generation time:  %.2f ms / %d tokens (%.2f ms/tok, %.2f tok/s)\n",
           timings.t_eval_ms,
           timings.n_eval,
           timings.t_eval_ms / timings.n_eval,
           1000.0f * timings.n_eval / timings.t_eval_ms);

    // 内存使用
    size_t used_mem = llama_get_used_mem(ctx);
    printf("  Memory used:      %.2f MB\n", used_mem / (1024.0 * 1024.0));
}
