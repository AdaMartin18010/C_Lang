/*
 * Auto-generated from: 11_Machine_Learning_C\03_Gradient_Descent_Optimizers.md
 * Line: 213
 * Language: c
 * Block ID: 7f0f7742
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    double* squared_grad_avg;  // E[g^2]
    double decay_rate;         // rho, 通常0.9
    double epsilon;
    double learning_rate;
} RMSprop_Optimizer;

void rmsprop_update(RMSprop_Optimizer* opt, double* params,
                    double* gradients, int n_params) {
    for (int i = 0; i < n_params; i++) {
        // E[g^2] = rho * E[g^2] + (1-rho) * g^2
        opt->squared_grad_avg[i] = opt->decay_rate * opt->squared_grad_avg[i]
                                  + (1 - opt->decay_rate) * gradients[i] * gradients[i];

        double adapted_lr = opt->learning_rate /
                           (sqrt(opt->squared_grad_avg[i]) + opt->epsilon);

        params[i] -= adapted_lr * gradients[i];
    }
}
