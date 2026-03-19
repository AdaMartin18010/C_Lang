/*
 * Auto-generated from: 00_VERSION_TRACKING\LLVM_22_C23_Update.md
 * Line: 1020
 * Language: c
 * Block ID: 25bf237c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// named_loops_example.c
// 演示 C2y Named Loops 的各种使用场景

#include <stdio.h>
#include <stdbool.h>

// 场景 1: 矩阵搜索
bool search_matrix(int rows, int cols,
                   int matrix[rows][cols],
                   int target,
                   int *found_row, int *found_col) {
    outer: for (int i = 0; i < rows; i++) {
        inner: for (int j = 0; j < cols; j++) {
            if (matrix[i][j] == target) {
                *found_row = i;
                *found_col = j;
                return true;
            }
            if (matrix[i][j] < 0) {
                continue outer;  // 跳过包含负数的行
            }
        }
    }
    return false;
}

// 场景 2: 多维数据处理
void process_3d_data(int x, int y, int z,
                     double data[x][y][z]) {
    x_loop: for (int i = 0; i < x; i++) {
        y_loop: for (int j = 0; j < y; j++) {
            z_loop: for (int k = 0; k < z; k++) {
                if (data[i][j][k] < 0.0) {
                    break x_loop;  // 发现异常，终止全部处理
                }
                if (data[i][j][k] == 0.0) {
                    continue y_loop;  // 跳过当前 y 平面
                }
                data[i][j][k] = transform(data[i][j][k]);
            }
        }
    }
}

// 场景 3: 复杂循环控制
void optimized_processing(int n, int m, int p) {
    stage1: for (int i = 0; i < n; i++) {
        if (!prepare_stage(i)) {
            break stage1;
        }

        stage2: for (int j = 0; j < m; j++) {
            if (skip_condition(i, j)) {
                continue stage1;  // 跳到外层循环的下一轮
            }

            stage3: for (int k = 0; k < p; k++) {
                if (early_exit_condition(i, j, k)) {
                    break stage2;  // 跳出中层循环
                }
                process(i, j, k);
            }
        }
    }
}

// 辅助函数声明
double transform(double x) { return x * x; }
bool prepare_stage(int i) { return i % 2 == 0; }
bool skip_condition(int i, int j) { return (i + j) % 3 == 0; }
bool early_exit_condition(int i, int j, int k) { return i + j + k > 100; }
void process(int i, int j, int k) {
    printf("Processing (%d, %d, %d)\n", i, j, k);
}

int main(void) {
    int matrix[5][5] = {
        {1, 2, 3, 4, 5},
        {6, 7, 8, 9, 10},
        {11, 12, 13, 14, 15},
        {16, 17, 18, 19, 20},
        {21, 22, 23, 24, 25}
    };

    int row, col;
    if (search_matrix(5, 5, matrix, 13, &row, &col)) {
        printf("Found 13 at (%d, %d)\n", row, col);
    }

    return 0;
}
