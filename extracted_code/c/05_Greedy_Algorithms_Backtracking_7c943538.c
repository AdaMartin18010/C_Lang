/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\05_Greedy_Algorithms_Backtracking.md
 * Line: 725
 * Language: c
 * Block ID: 7c943538
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ==================== N皇后问题 ====================

/**
 * @brief 检查在(row, col)放置皇后是否有效
 * @param board 棋盘，board[i]表示第i行皇后所在的列
 */
bool is_valid_queen_position(int board[], int row, int col) {
    for (int i = 0; i < row; i++) {
        // 检查同一列
        if (board[i] == col) return false;

        // 检查对角线（行差等于列差）
        if (abs(row - i) == abs(col - board[i])) return false;
    }
    return true;
}

/**
 * @brief N皇后回溯求解
 */
void backtrack_n_queens(int n, int board[], int row,
                        int **solutions, int *solution_count,
                        int max_solutions) {
    if (*solution_count >= max_solutions) return;

    // 找到一种解法
    if (row == n) {
        memcpy(solutions[*solution_count], board, n * sizeof(int));
        (*solution_count)++;
        return;
    }

    // 尝试在当前行的每一列放置皇后
    for (int col = 0; col < n; col++) {
        if (!is_valid_queen_position(board, row, col)) continue;

        // 放置皇后
        board[row] = col;

        // 递归下一行
        backtrack_n_queens(n, board, row + 1,
                           solutions, solution_count, max_solutions);

        // 回溯（不需要显式撤销，因为会被覆盖）
    }
}

int** solve_n_queens(int n, int *returnSize, int max_solutions) {
    // 最大解数估算
    int max_possible = (n == 8) ? 92 : 10000;
    if (max_solutions > max_possible) max_solutions = max_possible;

    int **solutions = malloc(max_solutions * sizeof(int *));
    for (int i = 0; i < max_solutions; i++) {
        solutions[i] = malloc(n * sizeof(int));
    }

    int *board = malloc(n * sizeof(int));

    *returnSize = 0;
    backtrack_n_queens(n, board, 0, solutions, returnSize, max_solutions);

    free(board);
    return solutions;
}

/**
 * @brief 打印N皇后解
 */
void print_n_queens_solution(int board[], int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf(board[i] == j ? "Q " : ". ");
        }
        printf("\n");
    }
    printf("\n");
}
