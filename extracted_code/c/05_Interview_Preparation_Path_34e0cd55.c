/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\05_Interview_Preparation_Path.md
 * Line: 481
 * Language: c
 * Block ID: 34e0cd55
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 缓存不友好的代码：
// 按列访问二维数组
int sum = 0;
for (int j = 0; j < cols; j++) {
    for (int i = 0; i < rows; i++) {
        sum += matrix[i][j];  // 跳跃访问，缓存失效
    }
}

// 缓存友好的代码：
// 按行访问，利用空间局部性
for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
        sum += matrix[i][j];  // 顺序访问，缓存命中
    }
}

// 其他优化技巧：
// 1. 数据对齐（64字节缓存行）
struct __attribute__((aligned(64))) CacheLine {
    int data[16];
};

// 2. 避免伪共享
struct ThreadData {
    int counter;
    char padding[60];  // 填充到64字节
};

// 3. 循环分块（Cache Blocking）
// 4. 预取指令 __builtin_prefetch
