/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\01_Digital_Logic_Gates.md
 * Line: 410
 * Language: c
 * Block ID: 9897300e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// n位行波进位加法器（Ripple Carry Adder）

typedef struct {
    uint32_t sum;
    Bool carry_out;
} AdderResult;

AddererResult ripple_carry_adder(uint32_t a, uint32_t b, Bool cin, int n_bits) {
    AdderResult result = {0, cin};

    for (int i = 0; i < n_bits; i++) {
        Bool bit_a = (a >> i) & 1;
        Bool bit_b = (b >> i) & 1;

        FullAdderResult fa = full_adder(bit_a, bit_b, result.carry_out);

        result.sum |= (fa.sum << i);
        result.carry_out = fa.carry;
    }

    return result;
}

// 延迟分析：n × 全加器延迟 = O(n)
// 32位加法器延迟 ≈ 32 × 100ps = 3.2ns
