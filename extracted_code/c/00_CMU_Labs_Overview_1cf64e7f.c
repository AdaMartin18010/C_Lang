/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\00_CMU_Labs_Overview.md
 * Line: 43
 * Language: c
 * Block ID: 1cf64e7f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 位操作示例
int bitAnd(int x, int y) {
    return ~(~x | ~y);  // 德摩根定律
}

int getByte(int x, int n) {
    return (x >> (n << 3)) & 0xFF;
}
