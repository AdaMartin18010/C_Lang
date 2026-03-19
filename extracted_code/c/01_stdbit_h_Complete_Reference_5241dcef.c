/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\C23_Standard_Library\01_stdbit_h_Complete_Reference.md
 * Line: 34
 * Language: c
 * Block ID: 5241dcef
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 引入 <stdbit.h> 提供标准化的位操作
// 替代之前的编译器特定扩展 (GCC __builtin_popcount, MSVC __popcnt)

#include <stdbit.h>
#include <stdio.h>

int main(void) {
    unsigned int x = 0b10110110;  // 182

    // 人口计数: 数出有多少个 1
    int ones = stdc_count_ones(x);        // 5

    // 前导零计数
    int leading = stdc_leading_zeros(x);  // 24 (假设 32 位)

    printf("ones=%d, leading_zeros=%d\n", ones, leading);
    return 0;
}
