/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 2724
 * Language: c
 * Block ID: 3781b18a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ✅ 易于优化
defer free(p);

// ⚠️ 复杂defer可能难以优化
defer {
    free(p);
    log_free(p);
    update_stats();
    notify_watchers();
};
