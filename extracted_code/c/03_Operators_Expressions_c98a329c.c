/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\03_Operators_Expressions.md
 * Line: 270
 * Language: c
 * Block ID: c98a329c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 常用位运算模式

// 1. 设置第n位
x |= (1 << n);

// 2. 清除第n位
x &= ~(1 << n);

// 3. 切换第n位
x ^= (1 << n);

// 4. 检查第n位
if (x & (1 << n)) { /* 第n位为1 */ }

// 5. 清除最低位的1
x &= (x - 1);

// 6. 获取最低位的1
lowbit = x & (-x);

// 7. 计算1的个数（popcount）
int popcount(uint32_t x) {
    x = (x & 0x55555555) + ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x & 0x0F0F0F0F) + ((x >> 4) & 0x0F0F0F0F);
    x = (x & 0x00FF00FF) + ((x >> 8) & 0x00FF00FF);
    x = (x & 0x0000FFFF) + ((x >> 16) & 0x0000FFFF);
    return x;
}

// 8. 判断2的幂
bool is_power_of_2(uint32_t x) {
    return x && !(x & (x - 1));
}

// 9. 取模优化（除数为2的幂）
// x % 16 == x & 15
// x / 16 == x >> 4
