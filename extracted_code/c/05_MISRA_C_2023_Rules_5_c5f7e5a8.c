/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\05_MISRA_C_2023_Rules_5.md
 * Line: 541
 * Language: c
 * Block ID: c5f7e5a8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 文档化位域 */
/**
 * Control register bit-field
 * Layout (MSB to LSB):
 *   [15:11] Reserved
 *   [10:8]  Mode (3 bits)
 *   [7]     Enable
 *   [6:0]   Prescaler (7 bits)
 * Total: 16 bits
 */
struct control_reg {
    uint16_t prescaler : 7;  /* [6:0] Clock prescaler value */
    uint16_t enable    : 1;  /* [7]   Module enable */
    uint16_t mode      : 3;  /* [10:8] Operating mode */
    uint16_t reserved  : 5;  /* [15:11] Reserved, write 0 */
};

/* ✅ 合规 - 显式使用uint类型 */
struct good {
    unsigned int sign : 1;  /* 明确无符号 */
};

/* ✅ 合规 - 配合静态断言检查大小 */
_Static_assert(sizeof(struct control_reg) == 2, "Must be 16 bits");
