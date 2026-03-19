/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\05_MISRA_C_2023_Rules_5.md
 * Line: 185
 * Language: c
 * Block ID: b635b65e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 保持const */
const int value = 10;
const int *p = &value;  /* 保持const */
/* *p = 20; */  /* 编译错误，正确！ */

/* ✅ 合规 - 保持volatile */
volatile uint32_t *status_reg = (volatile uint32_t *)0x40001000;
/* 始终通过volatile指针访问 */
uint32_t status = *status_reg;

/* ✅ 合规 - 如果需要修改，复制数据 */
const int config = get_config();
int working_copy = config;  /* 复制到非const变量 */
working_copy = modify(working_copy);

/* ✅ 合规 - 使用union（特殊情况） */
union data_conv {
    const uint8_t bytes[4];
    uint32_t word;
};
