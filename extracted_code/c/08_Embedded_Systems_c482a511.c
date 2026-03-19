/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\08_Embedded_Systems.md
 * Line: 231
 * Language: c
 * Block ID: c482a511
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 设置位 (置1)
REG |= (1 << n);           // 设置第n位
REG |= (1 << 3) | (1 << 5); // 设置第3和第5位

// 清除位 (置0)
REG &= ~(1 << n);          // 清除第n位
REG &= ~((1 << 3) | (1 << 5));

// 翻转位
REG ^= (1 << n);           // 翻转第n位

// 读取位
if (REG & (1 << n)) {      // 检查第n位是否为1
    // bit is set
}

// 修改位域 (多位)
// 先清除，再设置
REG &= ~(0xF << 4);        // 清除位4-7
REG |= (value << 4);       // 设置新的值

// 使用位域结构 (注意可移植性)
typedef union {
    uint32_t raw;
    struct {
        uint32_t EN : 1;      // 位0
        uint32_t MODE : 2;    // 位1-2
        uint32_t RESERVED : 29;
    } bits;
} Control_Reg;
