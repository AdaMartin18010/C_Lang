/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\07_Performance_Optimization.md
 * Line: 253
 * Language: c
 * Block ID: de5aabe7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 数组结构体 - 缓存不友好
typedef struct {
    int x;
    int y;
    int z;
} Point;

Point points[1000];

// 计算所有点的和
int sum_x = 0;
for (int i = 0; i < 1000; i++) {
    sum_x += points[i].x;  // 每次跳过12字节
}

// ✅ 结构体数组 - SoA (Structure of Arrays)
typedef struct {
    int *x;
    int *y;
    int *z;
} Points;

Points ps = {
    .x = malloc(1000 * sizeof(int)),
    .y = malloc(1000 * sizeof(int)),
    .z = malloc(1000 * sizeof(int))
};

// 连续访问
for (int i = 0; i < 1000; i++) {
    sum_x += ps.x[i];  // 完全顺序访问
}
