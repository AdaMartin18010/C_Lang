/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\05_Quantum_Random_Computing\README.md
 * Line: 539
 * Language: c
 * Block ID: 859f1550
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 随机性质量检验
 */

#include <string.h>

// 统计比特频率
void bit_frequency_test(const uint8_t *data, size_t len,
                         double *freq_0, double *freq_1)
{
    int count_0 = 0, count_1 = 0;
    for (size_t i = 0; i < len; i++) {
        for (int b = 0; b < 8; b++) {
            if ((data[i] >> b) & 1) {
                count_1++;
            } else {
                count_0++;
            }
        }
    }
    int total = count_0 + count_1;
    *freq_0 = (double)count_0 / total;
    *freq_1 = (double)count_1 / total;
}

// 游程检验
int runs_test(const uint8_t *data, size_t len)
{
    int runs = 1;
    int prev_bit = (data[0] & 1);

    for (size_t i = 0; i < len; i++) {
        for (int b = (i == 0) ? 1 : 0; b < 8; b++) {
            int bit = (data[i] >> b) & 1;
            if (bit != prev_bit) {
                runs++;
                prev_bit = bit;
            }
        }
    }

    return runs;
}
