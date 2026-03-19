/*
 * Auto-generated from: 11_Machine_Learning_C\03_Gradient_Descent_Optimizers.md
 * Line: 152
 * Language: c
 * Block ID: 83827ec9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void nesterov_update(Optimizer* opt, double* params,
                     double* gradients, int n_params) {
    for (int i = 0; i < n_params; i++) {
        double prev_velocity = opt->velocity[i];

        // 计算临时位置（展望）
        double lookahead = params[i] - opt->momentum * prev_velocity;

        // 在临时位置计算梯度
        double grad_at_lookahead = compute_gradient_at(lookahead);

        // 更新速度
        opt->velocity[i] = opt->momentum * prev_velocity
                         + opt->learning_rate * grad_at_lookahead;

        params[i] -= opt->velocity[i];
    }
}
