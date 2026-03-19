/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\01_Data_Lab.md
 * Line: 511
 * Language: c
 * Block ID: 7917d30d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
    unsigned sign, exp, frac, bias = 127;
    unsigned result;
    int shift;

    if (x == 0) return 0;

    // Handle INT_MIN specially
    if (x == 0x80000000) {
        return 0xCF000000;  // -2^31
    }

    sign = (x < 0) ? 0x80000000 : 0;
    if (x < 0) x = -x;

    // Find position of highest 1 bit
    shift = 0;
    unsigned temp = x;
    while (temp >> shift) {
        shift++;
    }
    shift--;

    exp = shift + bias;

    // Extract fraction bits (23 bits)
    if (shift <= 23) {
        frac = x << (23 - shift);
    } else {
        // Need to round
        int round_shift = shift - 23;
        frac = (x >> round_shift) & 0x7FFFFF;
        unsigned round_bits = x & ((1 << round_shift) - 1);
        unsigned half = 1 << (round_shift - 1);

        // Round to nearest even
        if (round_bits > half || (round_bits == half && (frac & 1))) {
            frac++;
            if (frac == 0x800000) {  // Overflow to next exponent
                frac = 0;
                exp++;
            }
        }
    }

    result = sign | (exp << 23) | frac;
    return result;
}
