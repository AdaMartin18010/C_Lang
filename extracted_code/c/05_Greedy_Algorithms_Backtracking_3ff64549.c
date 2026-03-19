/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\05_Greedy_Algorithms_Backtracking.md
 * Line: 883
 * Language: c
 * Block ID: 3ff64549
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ==================== 数独求解 ====================

#define SIZE 9
#define BOX_SIZE 3

bool is_valid_sudoku(char board[SIZE][SIZE], int row, int col, char num) {
    // 检查行
    for (int j = 0; j < SIZE; j++) {
        if (board[row][j] == num) return false;
    }

    // 检查列
    for (int i = 0; i < SIZE; i++) {
        if (board[i][col] == num) return false;
    }

    // 检查3x3方格
    int box_row = (row / BOX_SIZE) * BOX_SIZE;
    int box_col = (col / BOX_SIZE) * BOX_SIZE;
    for (int i = box_row; i < box_row + BOX_SIZE; i++) {
        for (int j = box_col; j < box_col + BOX_SIZE; j++) {
            if (board[i][j] == num) return false;
        }
    }

    return true;
}

bool solve_sudoku_backtrack(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == '.') {
                for (char num = '1'; num <= '9'; num++) {
                    if (is_valid_sudoku(board, i, j, num)) {
                        board[i][j] = num;

                        if (solve_sudoku_backtrack(board)) {
                            return true;
                        }

                        // 回溯
                        board[i][j] = '.';
                    }
                }
                return false;  // 没有数字可以填
            }
        }
    }
    return true;  // 所有格子都填满了
}

void print_sudoku(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        if (i % 3 == 0 && i > 0) {
            printf("------+-------+------\n");
        }
        for (int j = 0; j < SIZE; j++) {
            if (j % 3 == 0 && j > 0) printf("| ");
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
}
