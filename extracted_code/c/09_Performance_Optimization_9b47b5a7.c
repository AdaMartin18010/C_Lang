/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\09_Performance_Optimization.md
 * Line: 415
 * Language: c
 * Block ID: 9b47b5a7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 不好: 列优先访问 (缓存不友好)
for (int j = 0; j < cols; j++) {
    for (int i = 0; i < rows; i++) {
        sum += matrix[i][j];  // 跳跃访问
    }
}

// 好: 行优先访问 (缓存友好)
for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
        sum += matrix[i][j];  // 顺序访问
    }
}

// 结构体数组 vs 数组结构体
// 不好
struct Particle {
    float x, y, z;
    float vx, vy, vz;
} particles[1000];

// 可能更好 (如果只访问位置)
struct Particles {
    float x[1000], y[1000], z[1000];
    float vx[1000], vy[1000], vz[1000];
} particles;
