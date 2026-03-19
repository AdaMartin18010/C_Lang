/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\01_Data_Lab.md
 * Line: 585
 * Language: c
 * Block ID: 7cf78e68
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf) {
    unsigned sign = uf & 0x80000000;
    unsigned exp = (uf >> 23) & 0xFF;
    unsigned frac = uf & 0x007FFFFF;

    // NaN or Infinity
    if (exp == 0xFF) {
        return uf;
    }

    // Denormalized: shift fraction left
    if (exp == 0) {
        frac = frac << 1;
        // Check if becomes normalized
        if (frac & 0x00800000) {
            exp = 1;
            frac = frac & 0x007FFFFF;
        }
    } else {
        // Normalized: increment exponent
        exp++;
        // Check for overflow to infinity
        if (exp == 0xFF) {
            frac = 0;
        }
    }

    return sign | (exp << 23) | frac;
}
