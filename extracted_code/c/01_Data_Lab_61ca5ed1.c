/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\01_Data_Lab.md
 * Line: 148
 * Language: c
 * Block ID: 61ca5ed1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int logicalShift(int x, int n) {
    return (x >> n) & (~(((1 << 31) >> n) << 1));
}
