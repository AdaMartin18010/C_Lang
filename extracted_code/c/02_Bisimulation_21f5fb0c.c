/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\02_Coalgebraic_Methods\02_Bisimulation.md
 * Line: 430
 * Language: c
 * Block ID: 21f5fb0c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 计算epsilon闭包（tau闭包）
void epsilon_closure(LTS *lts, int state, bool *closure) {
    // BFS/DFS计算所有通过tau可达的状态
    bool *visited = calloc(lts_num_states(lts), sizeof(bool));
    int *stack = malloc(lts_num_states(lts) * sizeof(int));
    int top = 0;

    stack[top++] = state;
    visited[state] = true;

    while (top > 0) {
        int s = stack[--top];
        closure[s] = true;

        // 获取所有tau转移
        int *targets = lts_get_targets(lts, s, "tau");
        int tcount = lts_target_count(lts, s, "tau");

        for (int i = 0; i < tcount; i++) {
            if (!visited[targets[i]]) {
                visited[targets[i]] = true;
                stack[top++] = targets[i];
            }
        }
    }

    free(visited);
    free(stack);
}

// 计算弱转移
bool weak_transition(LTS *lts, int from, char *action, int to) {
    if (strcmp(action, "tau") == 0) {
        // tau动作：直接计算闭包
        bool *closure = calloc(lts_num_states(lts), sizeof(bool));
        epsilon_closure(lts, from, closure);
        bool result = closure[to];
        free(closure);
        return result;
    } else {
        // 可见动作：tau* a tau*
        bool *from_closure = calloc(lts_num_states(lts), sizeof(bool));
        epsilon_closure(lts, from, from_closure);

        // 对每个可达状态，检查a转移
        for (int i = 0; i < lts_num_states(lts); i++) {
            if (!from_closure[i]) continue;

            int *targets = lts_get_targets(lts, i, action);
            int tcount = lts_target_count(lts, i, action);

            for (int j = 0; j < tcount; j++) {
                bool *to_closure = calloc(lts_num_states(lts), sizeof(bool));
                epsilon_closure(lts, targets[j], to_closure);

                if (to_closure[to]) {
                    free(from_closure);
                    free(to_closure);
                    return true;
                }
                free(to_closure);
            }
        }

        free(from_closure);
        return false;
    }
}
