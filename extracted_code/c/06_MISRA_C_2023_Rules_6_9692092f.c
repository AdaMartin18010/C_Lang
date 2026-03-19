/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\06_MISRA_C_2023_Rules_6.md
 * Line: 396
 * Language: c
 * Block ID: 9692092f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 缺少default */
switch (status) {
    case OK:
        process_ok();
        break;
    case ERROR:
        process_error();
        break;
    /* 缺少default */
}

/* ❌ 违反 - 缺少break（fall-through）*/
switch (cmd) {
    case CMD_START:
        init();
        /* 缺少break，故意fall-through但无注释 */
    case CMD_RUN:
        run();
        break;
}

/* ❌ 违反 - default不是最后一个 */
switch (val) {
    case 1:
        break;
    default:
        break;
    case 2:  /* default后还有case */
        break;
}
