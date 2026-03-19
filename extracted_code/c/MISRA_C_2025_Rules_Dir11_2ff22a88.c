/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir11.md
 * Line: 518
 * Language: c
 * Block ID: 2ff22a88
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* switch语句必须完整 */

/* ❌ 违反 [待官方确认] - 缺少default */
switch (status) {
    case OK:
        process_ok();
        break;
    case ERROR:
        process_error();
        break;
    /* 缺少default */
}

/* ❌ 违反 [待官方确认] - 缺少break */
switch (cmd) {
    case CMD_START:
        init();
        /* 缺少break，无注释说明 */
    case CMD_RUN:
        run();
        break;
}

/* ✅ 合规 [待官方确认] */
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

/* ✅ 合规 [待官方确认] - 明确fall-through */
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
