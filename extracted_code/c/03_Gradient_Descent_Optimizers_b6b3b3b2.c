/*
 * Auto-generated from: 11_Machine_Learning_C\03_Gradient_Descent_Optimizers.md
 * Line: 127
 * Language: c
 * Block ID: b6b3b3b2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    double* velocity;
    double momentum;
    double learning_rate;
} Momentum_Optimizer;

void momentum_update(Momentum_Optimizer* opt, double* params,
                     double* gradients, int n_params) {
    for (int i = 0; i < n_params; i++) {
        // v = gamma * v + lr * grad
        opt->velocity[i] = opt->momentum * opt->velocity[i]
                         + opt->learning_rate * gradients[i];
        // theta = theta - v
        params[i] -= opt->velocity[i];
    }
}
