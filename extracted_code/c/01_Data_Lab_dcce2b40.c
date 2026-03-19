/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\01_Data_Lab.md
 * Line: 227
 * Language: c
 * Block ID: dcce2b40
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */
int bang(int x) {
    return ((x | (~x + 1)) >> 31) + 1;
}
