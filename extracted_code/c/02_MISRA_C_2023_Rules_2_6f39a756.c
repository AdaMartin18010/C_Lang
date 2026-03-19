/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\02_MISRA_C_2023_Rules_2.md
 * Line: 431
 * Language: c
 * Block ID: 6f39a756
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 所有标签都使用 */
void process(void)
{
    if (init() != OK) {
        goto error_handler;
    }

    if (setup() != OK) {
        goto error_handler;
    }

    do_work();
    return;

error_handler:
    cleanup();
    return;
}

/* ✅ 合规 - 完整的switch处理 */
void handle_state(enum State s)
{
    switch (s) {
    case STATE_INIT:
        init();
        break;
    case STATE_RUN:
        run();
        break;
    case STATE_STOP:
        stop();
        break;
    default:
        /* 处理意外状态 */
        break;
    }
}
