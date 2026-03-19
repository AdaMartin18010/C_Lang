/*
 * Auto-generated from: 11_Machine_Learning_C\03_Gradient_Descent_Optimizers.md
 * Line: 185
 * Language: c
 * Block ID: ef478846
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    double* squared_grad_sum;  // G
    double epsilon;
    double learning_rate;
} AdaGrad_Optimizer;

void adagrad_update(AdaGrad_Optimizer* opt, double* params,
                    double* gradients, int n_params) {
    for (int i = 0; i < n_params; i++) {
        // 累积平方梯度
        opt->squared_grad_sum[i] += gradients[i] * gradients[i];

        // 自适应学习率更新
        double adapted_lr = opt->learning_rate /
                           (sqrt(opt->squared_grad_sum[i]) + opt->epsilon);

        params[i] -= adapted_lr * gradients[i];
    }
}
