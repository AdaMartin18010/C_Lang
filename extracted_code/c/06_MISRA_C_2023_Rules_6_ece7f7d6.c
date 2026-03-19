/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\06_MISRA_C_2023_Rules_6.md
 * Line: 431
 * Language: c
 * Block ID: ece7f7d6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 完整的switch */
switch (status) {
    case OK:
        process_ok();
        break;
    case ERROR:
        process_error();
        break;
    default:
        /* 意外状态 */
        handle_unknown_status();
        break;
}

/* ✅ 合规 - 明确的fall-through */
switch (cmd) {
    case CMD_START:
        init();
        /* fall-through */
    case CMD_RUN:
        run();
        break;
    default:
        break;
}

/* ✅ 合规 - 枚举switch（穷尽检查）*/
enum color { RED, GREEN, BLUE };

switch (c) {
    case RED:
    case GREEN:
    case BLUE:
        break;
    default:
        /* 不应到达 */
        break;
}
