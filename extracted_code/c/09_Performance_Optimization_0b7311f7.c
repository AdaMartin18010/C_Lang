/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\09_Performance_Optimization.md
 * Line: 349
 * Language: c
 * Block ID: 0b7311f7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 优化1: 三角函数查找表
static const float sin_table[360] = {
    0.000000f, 0.017452f, 0.034899f, // ...
};

float fast_sin(int degrees) {
    return sin_table[degrees % 360];
}

// 优化2: CRC查找表
static const uint32_t crc32_table[256] = { /* ... */ };

uint32_t fast_crc32(const uint8_t *data, size_t len) {
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < len; i++) {
        crc = crc32_table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    }
    return ~crc;
}

// 优化3: 记忆化 (递归转DP)
int fib(int n) {
    static int memo[100] = {0};
    if (n <= 1) return n;
    if (memo[n] != 0) return memo[n];
    return memo[n] = fib(n - 1) + fib(n - 2);
}
