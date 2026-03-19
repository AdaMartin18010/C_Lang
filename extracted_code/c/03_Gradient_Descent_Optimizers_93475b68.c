/*
 * Auto-generated from: 11_Machine_Learning_C\03_Gradient_Descent_Optimizers.md
 * Line: 38
 * Language: c
 * Block ID: 93475b68
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    double* weights;
    double bias;
    double learning_rate;
} SGD_Optimizer;

// SGD更新
void sgd_update(SGD_Optimizer* opt, double* x, double y_true,
                double y_pred, int n_features) {
    double error = y_pred - y_true;

    for (int i = 0; i < n_features; i++) {
        double gradient = error * x[i];
        opt->weights[i] -= opt->learning_rate * gradient;
    }
    opt->bias -= opt->learning_rate * error;
}
