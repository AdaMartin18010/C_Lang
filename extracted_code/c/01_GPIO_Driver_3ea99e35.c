/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\Physical_Layer\01_GPIO_Driver.md
 * Line: 188
 * Language: c
 * Block ID: 3ea99e35
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 位带操作 - 原子性读写单个位
 * 适用于Cortex-M3/M4/M7
 */

/* 位带别名区计算 */
#define BITBAND_ADDR(addr, bitnum) (((addr) & 0xF0000000) + 0x2000000 + \
                                    (((addr) & 0xFFFFF) << 5) + ((bitnum) << 2))

#define MEM_ADDR(addr) *((volatile uint32_t *)(addr))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND_ADDR(addr, bitnum))

/* GPIO ODR位带 */
#define PAout(n)    BIT_ADDR(GPIOA_BASE + 0x14, n)   /* PAn输出 */
#define PAin(n)     BIT_ADDR(GPIOA_BASE + 0x10, n)   /* PAn输入 */

/* 使用示例 */
void bitband_example(void)
{
    /* 原子设置PA0 */
    PAout(0) = 1;

    /* 原子清除PA0 */
    PAout(0) = 0;

    /* 读取PA1 */
    uint8_t val = PAin(1);
}
