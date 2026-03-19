/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\08_Embedded_Systems.md
 * Line: 1268
 * Language: c
 * Block ID: cb402882
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 优化1: 用查找表代替计算
// 不好
float sin_approx(float x) {
    return x - x*x*x/6 + x*x*x*x*x/120;
}

// 好
const float sin_table[256] = { /* ... */ };
float sin_lookup(uint8_t angle) {
    return sin_table[angle];
}

// 优化2: 循环展开
// 不好
for (int i = 0; i < 100; i++) {
    process(data[i]);
}

// 好
for (int i = 0; i < 100; i += 4) {
    process(data[i]);
    process(data[i+1]);
    process(data[i+2]);
    process(data[i+3]);
}

// 优化3: 使用位运算
// 不好
if (value % 2 == 0)  // 除法

// 好
if ((value & 1) == 0)  // 位运算

// 优化4: 缓存友好
// 不好 (列优先访问)
for (int j = 0; j < cols; j++)
    for (int i = 0; i < rows; i++)
        matrix[i][j] = 0;

// 好 (行优先访问)
for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
        matrix[i][j] = 0;

// 优化5: 编译器提示
// __restrict 表示指针不重叠
void copy(int *__restrict dst, const int *__restrict src, int n) {
    for (int i = 0; i < n; i++) {
        dst[i] = src[i];
    }
}
