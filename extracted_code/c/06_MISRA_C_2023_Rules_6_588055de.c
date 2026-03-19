/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\06_MISRA_C_2023_Rules_6.md
 * Line: 474
 * Language: c
 * Block ID: 588055de
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* MISRA合规的Switch模板 */
switch (expression) {
    case VALUE_A:
        /* 处理A */
        action_a();
        break;

    case VALUE_B:
        /* 处理B */
        action_b();
        break;

    case VALUE_C:
        /* 无break，明确fall-through */
        prepare_for_d();
        /* fall-through */

    case VALUE_D:
        action_cd();
        break;

    default:
        /* 防御性编程 */
        handle_unexpected();
        break;
}
