/*
 * Auto-generated from: 11_Machine_Learning_C\03_Gradient_Descent_Optimizers.md
 * Line: 247
 * Language: c
 * Block ID: 4d88998a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    double* m;           // 一阶矩估计
    double* v;           // 二阶矩估计
    double beta1;        // 通常0.9
    double beta2;        // 通常0.999
    double epsilon;      // 通常1e-8
    double learning_rate;
    int t;               // 时间步
} Adam_Optimizer;

Adam_Optimizer* adam_create(int n_params, double lr) {
    Adam_Optimizer* opt = malloc(sizeof(Adam_Optimizer));
    opt->m = calloc(n_params, sizeof(double));
    opt->v = calloc(n_params, sizeof(double));
    opt->beta1 = 0.9;
    opt->beta2 = 0.999;
    opt->epsilon = 1e-8;
    opt->learning_rate = lr;
    opt->t = 0;
    return opt;
}

void adam_update(Adam_Optimizer* opt, double* params,
                 double* gradients, int n_params) {
    opt->t++;

    for (int i = 0; i < n_params; i++) {
        // 更新一阶和二阶矩
        opt->m[i] = opt->beta1 * opt->m[i] + (1 - opt->beta1) * gradients[i];
        opt->v[i] = opt->beta2 * opt->v[i] + (1 - opt->beta2) * gradients[i] * gradients[i];

        // 偏差修正
        double m_hat = opt->m[i] / (1 - pow(opt->beta1, opt->t));
        double v_hat = opt->v[i] / (1 - pow(opt->beta2, opt->t));

        // 参数更新
        params[i] -= opt->learning_rate * m_hat / (sqrt(v_hat) + opt->epsilon);
    }
}
