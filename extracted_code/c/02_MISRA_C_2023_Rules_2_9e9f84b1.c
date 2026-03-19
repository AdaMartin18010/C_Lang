/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\02_MISRA_C_2023_Rules_2.md
 * Line: 396
 * Language: c
 * Block ID: 9e9f84b1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 Rule 2.4 */
void process(void)
{
    if (init() != OK) {
        goto error_handler;
    }

    do_work();
    return;

error_handler:  /* 如果所有错误都在if内处理，这个标签可能未使用 */
    cleanup();
    return;
}

/* ❌ 违反 Rule 2.4 - switch中的未使用case */
enum State { STATE_INIT, STATE_RUN, STATE_STOP };

void handle_state(enum State s)
{
    switch (s) {
    case STATE_INIT:
        init();
        break;
    case STATE_RUN:
        run();
        break;
    /* STATE_STOP 未处理 */
    }
}
