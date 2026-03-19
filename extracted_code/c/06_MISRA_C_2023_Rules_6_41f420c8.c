/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\06_MISRA_C_2023_Rules_6.md
 * Line: 685
 * Language: c
 * Block ID: 41f420c8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 使用空体注释 */
while (*ptr != '\0') {
    ptr++;  /* 明确的操作 */
}

/* ✅ 合规 - 空循环明确 */
while (is_busy()) {
    /* 等待硬件就绪 */
}

/* ✅ 合规 - 使用(void)抑制未使用参数 */
void callback(void *unused)
{
    (void)unused;  /* 明确抑制警告 */
}
