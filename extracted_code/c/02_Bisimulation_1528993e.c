/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\02_Coalgebraic_Methods\02_Bisimulation.md
 * Line: 157
 * Language: c
 * Block ID: 1528993e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 不动点算法计算最大互模拟
Bisimulation *naive_bisimulation(LTS *lts) {
    int n = lts_num_states(lts);

    // 初始：所有状态对都相关
    bool **R = calloc(n, sizeof(bool *));
    for (int i = 0; i < n; i++) {
        R[i] = calloc(n, sizeof(bool));
        for (int j = 0; j < n; j++) {
            R[i][j] = true;  // 初始假设所有状态等价
        }
    }

    bool changed;
    do {
        changed = false;
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                if (!R[i][j]) continue;

                // 检查(i,j)是否满足互模拟条件
                if (!check_bisim_condition(lts, R, i, j)) {
                    R[i][j] = R[j][i] = false;
                    changed = true;
                }
            }
        }
    } while (changed);

    // 转换回Bisimulation结构
    return matrix_to_bisimulation(R, n);
}

// 检查单个状态对是否满足条件
bool check_bisim_condition(LTS *lts, bool **R, int p, int q) {
    // 检查所有动作
    char **actions = lts_get_actions(lts);
    int num_actions = lts_num_actions(lts);

    for (int a = 0; a < num_actions; a++) {
        char *action = actions[a];

        // p能执行a到达的所有状态
        int *p_targets = lts_get_targets(lts, p, action);
        int p_count = lts_target_count(lts, p, action);

        // q能执行a到达的所有状态
        int *q_targets = lts_get_targets(lts, q, action);
        int q_count = lts_target_count(lts, q, action);

        // 检查每个p'是否存在匹配的q'
        for (int i = 0; i < p_count; i++) {
            bool found = false;
            for (int j = 0; j < q_count; j++) {
                if (R[p_targets[i]][q_targets[j]]) {
                    found = true;
                    break;
                }
            }
            if (!found) return false;
        }

        // 对称检查
        for (int j = 0; j < q_count; j++) {
            bool found = false;
            for (int i = 0; i < p_count; i++) {
                if (R[q_targets[j]][p_targets[i]]) {
                    found = true;
                    break;
                }
            }
            if (!found) return false;
        }
    }

    return true;
}

// 复杂度: O(n^4 * m) 其中n是状态数，m是动作数
