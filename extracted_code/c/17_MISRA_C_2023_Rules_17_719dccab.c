/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\17_MISRA_C_2023_Rules_17.md
 * Line: 125
 * Language: c
 * Block ID: 719dccab
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 无符号位域 */
struct ControlRegister {
    uint32_t enable    : 1;  /* bit 0 */
    uint32_t mode      : 3;  /* bits 1-3 */
    uint32_t prescaler : 4;  /* bits 4-7 */
    uint32_t reserved  : 24; /* bits 8-31 */
};

/* ✅ 合规 - 文档化位域布局 */
/**
 * Status register bit layout
 *
 *  31      24 23      16 15       8 7        0
 * +----------+----------+----------+----------+
 * |  reserved  |   error    |  warning   |   ready   |
 * +----------+----------+----------+----------+
 */
struct StatusReg {
    uint32_t ready   : 8;  /* [7:0]   Ready flags */
    uint32_t warning : 8;  /* [15:8]  Warning flags */
    uint32_t error   : 8;  /* [23:16] Error flags */
    uint32_t reserved: 8;  /* [31:24] Reserved */
};

/* ✅ 合规 - 使用掩码操作（可移植）*/
#define CTRL_ENABLE_POS    0
#define CTRL_ENABLE_MASK   (1U << CTRL_ENABLE_POS)
#define CTRL_MODE_POS      1
#define CTRL_MODE_MASK     (7U << CTRL_MODE_POS)

static inline void set_enable(uint32_t *reg, bool enable)
{
    if (enable) {
        *reg |= CTRL_ENABLE_MASK;
    } else {
        *reg &= ~CTRL_ENABLE_MASK;
    }
}

static inline void set_mode(uint32_t *reg, uint32_t mode)
{
    *reg = (*reg & ~CTRL_MODE_MASK) | ((mode << CTRL_MODE_POS) & CTRL_MODE_MASK);
}
