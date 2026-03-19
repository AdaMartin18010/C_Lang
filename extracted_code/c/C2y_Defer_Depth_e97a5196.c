/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 2678
 * Language: c
 * Block ID: e97a5196
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 反模式：defer中包含业务逻辑
defer {
    if (data_processed) {
        save_to_database();
        notify_user();
        update_cache();
    }
};

// ✅ defer只应该处理清理
defer cleanup_resources();

// 业务逻辑在正常流程中
if (data_processed) {
    save_to_database();
    notify_user();
    update_cache();
}
