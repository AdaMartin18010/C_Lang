/*
 * C2y Named Loops 示例
 * 编译: gcc -std=c2y -o 15_named_loops 15_named_loops.c
 *       clang -std=c2y -o 15_named_loops 15_named_loops.c
 */

#include <stdio.h>
#include <stdbool.h>

// 基本的命名循环
void basic_named_loops(void) {
    printf("Basic named loops:\n");
    
    outer: for (int i = 0; i < 3; i++) {
        inner: for (int j = 0; j < 3; j++) {
            printf("(%d,%d) ", i, j);
            if (j == 1) {
                continue inner;  // 继续内层循环（显式）
            }
        }
        printf("\n");
    }
}

// 使用 break 跳出外层循环
void break_outer_loop(void) {
    printf("\nBreak outer loop:\n");
    
    int matrix[5][5] = {
        {1, 2, 3, 4, 5},
        {6, 7, 8, 9, 10},
        {11, 12, 13, 14, 15},
        {16, 17, 18, 19, 20},
        {21, 22, 23, 24, 25}
    };
    
    int target = 13;
    bool found = false;
    int found_i = -1, found_j = -1;
    
    search: for (int i = 0; i < 5; i++) {
        scan: for (int j = 0; j < 5; j++) {
            if (matrix[i][j] == target) {
                found = true;
                found_i = i;
                found_j = j;
                break search;  // 跳出外层循环
            }
        }
    }
    
    if (found) {
        printf("Found %d at [%d][%d]\n", target, found_i, found_j);
    }
}

// 搜索并跳过整行
void skip_row_example(void) {
    printf("\nSkip row example:\n");
    
    // 假设矩阵每行已排序
    int sorted_matrix[5][5] = {
        {1, 3, 5, 7, 9},
        {2, 4, 6, 8, 10},
        {11, 13, 15, 17, 19},
        {12, 14, 16, 18, 20},
        {21, 22, 23, 24, 25}
    };
    
    int target = 15;
    
    rows: for (int i = 0; i < 5; i++) {
        cols: for (int j = 0; j < 5; j++) {
            if (sorted_matrix[i][j] == target) {
                printf("Found %d at row %d, col %d\n", target, i, j);
                return;
            }
            if (sorted_matrix[i][j] > target) {
                printf("Skipping remaining of row %d (value %d > %d)\n", 
                       i, sorted_matrix[i][j], target);
                break cols;  // 跳过当前行的剩余元素
            }
        }
    }
}

// 处理多维数据
void process_3d_data(void) {
    printf("\n3D data processing:\n");
    
    int cube[3][3][3];
    int value = 1;
    
    // 初始化
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                cube[i][j][k] = value++;
            }
        }
    }
    
    // 查找并处理
    int target = 15;
    
    depth: for (int i = 0; i < 3; i++) {
        row: for (int j = 0; j < 3; j++) {
            col: for (int k = 0; k < 3; k++) {
                if (cube[i][j][k] == target) {
                    printf("Found %d at [%d][%d][%d]\n", target, i, j, k);
                    printf("Neighbors: ");
                    
                    // 打印邻居
                    if (i > 0) printf("%d ", cube[i-1][j][k]);
                    if (i < 2) printf("%d ", cube[i+1][j][k]);
                    if (j > 0) printf("%d ", cube[i][j-1][k]);
                    if (j < 2) printf("%d ", cube[i][j+1][k]);
                    printf("\n");
                    
                    break depth;  // 找到后立即停止
                }
            }
        }
    }
}

// while 循环命名
void named_while_loop(void) {
    printf("\nNamed while loop:\n");
    
    int i = 0;
    outer: while (i < 3) {
        int j = 0;
        inner: while (j < 3) {
            printf("(%d,%d) ", i, j);
            if (++j >= 2) {
                break inner;
            }
        }
        printf("\n");
        i++;
    }
}

// do-while 循环命名
void named_do_while(void) {
    printf("\nNamed do-while loop:\n");
    
    int i = 0;
    outer: do {
        int j = 0;
        inner: do {
            printf("[%d,%d] ", i, j);
        } while (++j < 2);
        printf("\n");
    } while (++i < 2);
}

// 复杂控制流
void complex_control_flow(void) {
    printf("\nComplex control flow:\n");
    
    int data[4][4] = {
        {1, 0, 3, 4},
        {5, 6, 0, 8},
        {9, 10, 11, 0},
        {13, 14, 15, 16}
    };
    
    // 查找第一个零并报告位置
    matrix: for (int i = 0; i < 4; i++) {
        scan: for (int j = 0; j < 4; j++) {
            if (data[i][j] == 0) {
                printf("First zero at [%d][%d]\n", i, j);
                // 继续扫描这一行的更多零
                continue scan;
            }
        }
    }
}

int main(void) {
    printf("C2y Named Loops Demo\n");
    printf("====================\n");
    
    basic_named_loops();
    break_outer_loop();
    skip_row_example();
    process_3d_data();
    named_while_loop();
    named_do_while();
    complex_control_flow();
    
    printf("\nNamed loops demo complete!\n");
    
    return 0;
}
