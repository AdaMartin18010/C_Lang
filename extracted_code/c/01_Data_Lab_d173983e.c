/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\01_Data_Lab.md
 * Line: 170
 * Language: c
 * Block ID: d173983e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int logicalShift(int x, int n) {
    int mask = ~(((0x1 << 31) >> n) << 1);
    return (x >> n) & mask;
}
