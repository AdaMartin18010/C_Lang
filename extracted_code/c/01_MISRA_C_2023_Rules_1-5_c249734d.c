/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\01_MISRA_C_2023_Rules_1-5.md
 * Line: 175
 * Language: c
 * Block ID: c249734d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 错误码+恢复机制 */
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
