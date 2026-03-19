/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\01_Data_Lab.md
 * Line: 183
 * Language: c
 * Block ID: d22cb1dc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
    int mask1 = 0x55 | (0x55 << 8);
    int mask2 = 0x33 | (0x33 << 8);
    int mask3 = 0x0F | (0x0F << 8);
    int mask4 = 0xFF | (0xFF << 8);
    int mask5 = 0xFF | (0xFF << 16);

    mask1 = mask1 | (mask1 << 16);  // 0x55555555
    mask2 = mask2 | (mask2 << 16);  // 0x33333333
    mask3 = mask3 | (mask3 << 16);  // 0x0F0F0F0F
    mask4 = mask4 | (mask4 << 16);  // 0x00FF00FF

    x = (x & mask1) + ((x >> 1) & mask1);
    x = (x & mask2) + ((x >> 2) & mask2);
    x = (x & mask3) + ((x >> 4) & mask3);
    x = (x & mask4) + ((x >> 8) & mask4);
    x = (x & mask5) + ((x >> 16) & mask5);

    return x;
}
