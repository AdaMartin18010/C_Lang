/*
 * Auto-generated from: 12_Practice_Exercises\07_Algorithm_Challenges.md
 * Line: 276
 * Language: c
 * Block ID: d613a526
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    uint32_t *digits;  // 小端序，每元素存32位
    size_t size;       // 有效位数
    size_t capacity;
    bool negative;
} BigInt;

BigInt* bigint_from_int(int64_t n);
BigInt* bigint_from_string(const char *str);
char* bigint_to_string(const BigInt *n);
void bigint_free(BigInt *n);

BigInt* bigint_add(const BigInt *a, const BigInt *b);
BigInt* bigint_sub(const BigInt *a, const BigInt *b);
BigInt* bigint_mul(const BigInt *a, const BigInt *b);
BigInt* bigint_div(const BigInt *a, const BigInt *b, BigInt **remainder);
BigInt* bigint_pow(const BigInt *base, uint64_t exp);
BigInt* bigint_mod_pow(const BigInt *base, const BigInt *exp,
                       const BigInt *mod);  // RSA需要
