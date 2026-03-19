/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\08_Code_Review_Checklist.md
 * Line: 772
 * Language: c
 * Block ID: 75c15f54
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误示例：缓存不友好（列优先访问）
void matrix_multiply_slow(int **a, int **b, int **c, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            c[i][j] = 0;
            for (int k = 0; k < n; k++) {
                // 访问b[k][j]是跳跃式的（缓存不友好）
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

// ✅ 优化后：缓存友好 + 分块优化
#define BLOCK_SIZE 64

void matrix_multiply_fast(int a[][N], int b[][N], int c[][N], int n) {
    // 初始化结果矩阵
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            c[i][j] = 0;
        }
    }

    // 分块矩阵乘法
    for (int ii = 0; ii < n; ii += BLOCK_SIZE) {
        for (int jj = 0; jj < n; jj += BLOCK_SIZE) {
            for (int kk = 0; kk < n; kk += BLOCK_SIZE) {
                // 处理一个块
                for (int i = ii; i < ii + BLOCK_SIZE && i < n; i++) {
                    for (int j = jj; j < jj + BLOCK_SIZE && j < n; j++) {
                        int sum = c[i][j];
                        for (int k = kk; k < kk + BLOCK_SIZE && k < n; k++) {
                            sum += a[i][k] * b[k][j];
                        }
                        c[i][j] = sum;
                    }
                }
            }
        }
    }
}

// ❌ 错误示例：结构体数组缓存不友好
struct particle_bad {
    double x, y, z;      // 位置
    double vx, vy, vz;   // 速度
    char name[32];       // 名称（很少访问）
    double mass;         // 质量
};

// ✅ 优化后：结构体字段重排（热数据在一起）
struct particle_good {
    double x, y, z;      // 位置（热数据）
    double vx, vy, vz;   // 速度（热数据）
    double mass;         // 质量（热数据）
    char name[32];       // 名称（冷数据，放在最后）
};
