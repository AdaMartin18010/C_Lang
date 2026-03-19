/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\C23_Standard_Library\01_stdbit_h_Complete_Reference.md
 * Line: 114
 * Language: c
 * Block ID: e705a321
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 软件实现 (当硬件 POPCNT 不可用时)
// 分治算法
int popcount_software(uint32_t x) {
    x = (x & 0x55555555) + ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x & 0x0F0F0F0F) + ((x >> 4) & 0x0F0F0F0F);
    x = (x & 0x00FF00FF) + ((x >> 8) & 0x00FF00FF);
    x = (x & 0x0000FFFF) + ((x >> 16) & 0x0000FFFF);
    return x;
}

// 更快的实现 (使用乘法)
int popcount_fast(uint32_t x) {
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = ((x + (x >> 4)) & 0x0F0F0F0F);
    return (x * 0x01010101) >> 24;
}
