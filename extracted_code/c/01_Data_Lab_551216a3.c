/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\01_Data_Lab.md
 * Line: 419
 * Language: c
 * Block ID: 551216a3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
int ilog2(int x) {
    int result = 0;
    int mask;

    // Check if any bits in high 16 bits are set
    mask = !!(x >> 16);
    result = result + (mask << 4);
    x = x >> (mask << 4);

    // Check high 8 bits of remaining
    mask = !!(x >> 8);
    result = result + (mask << 3);
    x = x >> (mask << 3);

    // Check high 4 bits
    mask = !!(x >> 4);
    result = result + (mask << 2);
    x = x >> (mask << 2);

    // Check high 2 bits
    mask = !!(x >> 2);
    result = result + (mask << 1);
    x = x >> (mask << 1);

    // Check high 1 bit
    mask = !!(x >> 1);
    result = result + mask;

    return result;
}
