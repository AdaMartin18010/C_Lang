/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\02_MISRA_C_2023_Rules_2.md
 * Line: 151
 * Language: c
 * Block ID: 7d638e81
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 删除不可达代码 */
int calculate(int x)
{
    if (x < 0) {
        return -1;
    }
    return x * 2;
}

/* ✅ 合规 - 使用条件编译 */
#define MODE_A 1

void configure(void)
{
    #if MODE_A
        setup_mode_a();
    #else
        setup_mode_b();
    #endif
}

/* ✅ 合规 - 死循环中的清理 */
_Noreturn void run_forever(void)
{
    while (1) {
        process_data();
        /* 如果需要清理，在循环内处理 */
    }
}
