/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\01_Game_Semantics\01_Game_Semantics_Theory.md
 * Line: 345
 * Language: c
 * Block ID: d975d496
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 验证两个程序是否语境等价
bool contextual_equivalence_check(
    Strategy *sem_M,    // M的语义
    Strategy *sem_N,    // N的语义
    Arena *arena
) {
    // 检查所有O策略
    for (int i = 0; i < num_opponent_strategies; i++) {
        Strategy *alpha = opponent_strategies[i];

        // 交互结果
        InteractionResult *result_M = interact(alpha, sem_M);
        InteractionResult *result_N = interact(alpha, sem_N);

        // 比较结果
        if (!results_equal(result_M, result_N)) {
            // 找到反例
            printf("不等价! 反例策略: %d\n", i);
            return false;
        }
    }

    return true;
}
