/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir1.md
 * Line: 292
 * Language: c
 * Block ID: 01ca69ff
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 [C:2023延续] - 错误码+恢复机制 */
typedef enum {
    ERR_NONE = 0,
    ERR_HARDWARE,
    ERR_MEMORY
} error_t;

static volatile error_t g_last_error = ERR_NONE;

void handle_error(error_t error)
{
    g_last_error = error;
    log_error(error);
    /* 尝试恢复或进入安全状态 */
    enter_safe_state();
}

/* 不使用exit/abort */

/* ✅ 合规 [C23相关] - 错误处理 */
void process_with_c23_features(void) {
#if __STDC_VERSION__ >= 202311L
    constexpr int MAX_RETRIES = 3;
#else
    const int MAX_RETRIES = 3;
#endif

    for (int i = 0; i < MAX_RETRIES; i++) {
        if (try_operation()) {
            return;  /* 成功 */
        }
    }
    handle_error(ERR_HARDWARE);  /* 错误处理，不退出 */
}
