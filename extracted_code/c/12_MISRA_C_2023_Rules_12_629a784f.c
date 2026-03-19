/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\12_MISRA_C_2023_Rules_12.md
 * Line: 157
 * Language: c
 * Block ID: 629a784f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 缺少default */
switch (value) {
    case 1: do_something(); break;
    case 2: do_other(); break;
    /* 缺少default */
}

/* ❌ 违反 - default不在最后 */
switch (value) {
    case 1: break;
    default: break;
    case 2: break;  /* default后还有case */
}

/* ❌ 违反 - 隐式fall-through */
switch (cmd) {
    case CMD_A:
        init();  /* 缺少break！ */
    case CMD_B:
        process();
        break;
}
