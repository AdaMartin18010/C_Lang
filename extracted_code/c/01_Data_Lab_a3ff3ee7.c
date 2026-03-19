/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\01_Data_Lab.md
 * Line: 473
 * Language: c
 * Block ID: a3ff3ee7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_neg(unsigned uf) {
    unsigned mask = 0x80000000;
    unsigned result = uf ^ mask;  // Flip sign bit
    unsigned exp = uf & 0x7F800000;
    unsigned frac = uf & 0x007FFFFF;

    // Check for NaN: exp all 1s and frac non-zero
    if (exp == 0x7F800000 && frac != 0) {
        return uf;
    }
    return result;
}
