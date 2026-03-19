/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\01_Data_Lab.md
 * Line: 384
 * Language: c
 * Block ID: 77c13b6f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * isLessOrEqual - if x <= y  then return 1, else return 0
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
    int signX = (x >> 31) & 1;
    int signY = (y >> 31) & 1;
    int signDiff = signX ^ signY;  // 1 if signs differ

    // If signs differ, x <= y only if x is negative
    // If signs same, y - x >= 0
    int diff = y + (~x + 1);  // y - x
    int signDiffResult = signX;  // signs differ, result is sign of x
    int sameSignResult = !(diff >> 31);  // signs same, check y >= x

    return (signDiff & signDiffResult) | ((~signDiff) & sameSignResult);
}
