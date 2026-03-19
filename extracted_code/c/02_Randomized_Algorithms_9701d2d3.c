/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\05_Quantum_Random_Computing\02_Randomized_Algorithms.md
 * Line: 467
 * Language: c
 * Block ID: 9701d2d3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Morris计数器：使用O(log log n)空间近似计数
typedef struct {
    uint8_t exponent;  // 计数近似为 2^exponent / 0.69
} MorrisCounter;

void morris_init(MorrisCounter *c) {
    c->exponent = 0;
}

void morris_increment(MorrisCounter *c) {
    // 以概率 1/2^exponent 增加exponent
    double prob = 1.0 / (1 << c->exponent);
    if ((double)rand() / RAND_MAX < prob) {
        c->exponent++;
    }
}

uint64_t morris_estimate(const MorrisCounter *c) {
    // E[count] ≈ (2^exponent - 1) / ln(2)
    return (uint64_t)((exp2(c->exponent) - 1) / 0.693147);
}

// 使用多个计数器减少方差
typedef struct {
    MorrisCounter counters[64];
} MorrisCounterArray;

void morris_array_increment(MorrisCounterArray *arr) {
    for (int i = 0; i < 64; i++) {
        morris_increment(&arr->counters[i]);
    }
}

uint64_t morris_array_estimate(const MorrisCounterArray *arr) {
    uint64_t sum = 0;
    for (int i = 0; i < 64; i++) {
        sum += morris_estimate(&arr->counters[i]);
    }
    return sum / 64;
}
