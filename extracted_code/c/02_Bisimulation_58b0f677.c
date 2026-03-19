/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\02_Coalgebraic_Methods\02_Bisimulation.md
 * Line: 503
 * Language: c
 * Block ID: 58b0f677
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 弱互模拟检查
bool is_weak_bisimulation(LTS *lts, Bisimulation *R) {
    for (BisimPair *pair = R->pairs; pair; pair = pair->next) {
        int p = pair->state1;
        int q = pair->state2;

        char **actions = lts_get_visible_actions(lts);
        int num_actions = lts_num_visible_actions(lts);

        // 检查每个可见动作
        for (int a = 0; a < num_actions; a++) {
            char *action = actions[a];

            // p的弱转移必须在q中有匹配
            for (int p_prime = 0; p_prime < lts_num_states(lts); p_prime++) {
                if (!weak_transition(lts, p, action, p_prime)) continue;

                bool found = false;
                for (int q_prime = 0; q_prime < lts_num_states(lts); q_prime++) {
                    if (!weak_transition(lts, q, action, q_prime)) continue;
                    if (pair_in_bisim(R, p_prime, q_prime)) {
                        found = true;
                        break;
                    }
                }
                if (!found) return false;
            }

            // 对称检查...
        }
    }
    return true;
}
