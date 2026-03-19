/*
 * Auto-generated from: 11_Machine_Learning_C\01_Neural_Network_Basics.md
 * Line: 550
 * Language: c
 * Block ID: d35df26f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 优化器类型
typedef enum {
    OPT_SGD,
    OPT_MOMENTUM,
    OPT_ADAM,
    OPT_ADAMW
} OptimizerType;

// 优化器状态
typedef struct {
    OptimizerType type;
    double learning_rate;
    double momentum;        // Momentum系数
    double beta1;           // Adam β1
    double beta2;           // Adam β2
    double epsilon;
    double weight_decay;    // AdamW
    int t;                  // 时间步

    // 一阶矩估计 (momentum)
    double** m_weights;     // [layer][i][j]
    double** m_biases;

    // 二阶矩估计 (adaptive)
    double** v_weights;
    double** v_biases;
} Optimizer;

// SGD更新
void optimizer_step_sgd(Optimizer* opt, Layer* layer) {
    for (int i = 0; i < layer->n_outputs; i++) {
        // 更新偏置
        layer->biases[i] -= opt->learning_rate * layer->bias_grads[i];

        // 更新权重
        for (int j = 0; j < layer->n_inputs; j++) {
            layer->weights[i][j] -= opt->learning_rate * layer->weight_grads[i][j];
        }
    }
}

// Momentum更新
void optimizer_step_momentum(Optimizer* opt, Layer* layer, int layer_idx) {
    for (int i = 0; i < layer->n_outputs; i++) {
        // 偏置: v = βv + (1-β)g
        opt->m_biases[layer_idx][i] = opt->momentum * opt->m_biases[layer_idx][i]
                                      + layer->bias_grads[i];
        layer->biases[i] -= opt->learning_rate * opt->m_biases[layer_idx][i];

        for (int j = 0; j < layer->n_inputs; j++) {
            opt->m_weights[layer_idx][i][j] = opt->momentum * opt->m_weights[layer_idx][i][j]
                                              + layer->weight_grads[i][j];
            layer->weights[i][j] -= opt->learning_rate * opt->m_weights[layer_idx][i][j];
        }
    }
}

// Adam更新
void optimizer_step_adam(Optimizer* opt, Layer* layer, int layer_idx) {
    opt->t++;
    double lr_t = opt->learning_rate * sqrt(1 - pow(opt->beta2, opt->t))
                  / (1 - pow(opt->beta1, opt->t));

    for (int i = 0; i < layer->n_outputs; i++) {
        // 偏置更新
        double g = layer->bias_grads[i];
        opt->m_biases[layer_idx][i] = opt->beta1 * opt->m_biases[layer_idx][i]
                                      + (1 - opt->beta1) * g;
        opt->v_biases[layer_idx][i] = opt->beta2 * opt->v_biases[layer_idx][i]
                                      + (1 - opt->beta2) * g * g;
        double m_hat = opt->m_biases[layer_idx][i] / (1 - pow(opt->beta1, opt->t));
        double v_hat = opt->v_biases[layer_idx][i] / (1 - pow(opt->beta2, opt->t));
        layer->biases[i] -= lr_t * m_hat / (sqrt(v_hat) + opt->epsilon);

        for (int j = 0; j < layer->n_inputs; j++) {
            g = layer->weight_grads[i][j];
            opt->m_weights[layer_idx][i][j] = opt->beta1 * opt->m_weights[layer_idx][i][j]
                                              + (1 - opt->beta1) * g;
            opt->v_weights[layer_idx][i][j] = opt->beta2 * opt->v_weights[layer_idx][i][j]
                                              + (1 - opt->beta2) * g * g;
            m_hat = opt->m_weights[layer_idx][i][j] / (1 - pow(opt->beta1, opt->t));
            v_hat = opt->v_weights[layer_idx][i][j] / (1 - pow(opt->beta2, opt->t));
            layer->weights[i][j] -= lr_t * m_hat / (sqrt(v_hat) + opt->epsilon);
        }
    }
}

// AdamW更新 (权重衰减与梯度更新解耦)
void optimizer_step_adamw(Optimizer* opt, Layer* layer, int layer_idx) {
    opt->t++;
    double lr_t = opt->learning_rate * sqrt(1 - pow(opt->beta2, opt->t))
                  / (1 - pow(opt->beta1, opt->t));

    for (int i = 0; i < layer->n_outputs; i++) {
        // 权重衰减 (在梯度步骤之前)
        layer->biases[i] *= (1.0 - opt->learning_rate * opt->weight_decay);

        double g = layer->bias_grads[i];
        opt->m_biases[layer_idx][i] = opt->beta1 * opt->m_biases[layer_idx][i]
                                      + (1 - opt->beta1) * g;
        opt->v_biases[layer_idx][i] = opt->beta2 * opt->v_biases[layer_idx][i]
                                      + (1 - opt->beta2) * g * g;
        double m_hat = opt->m_biases[layer_idx][i] / (1 - pow(opt->beta1, opt->t));
        double v_hat = opt->v_biases[layer_idx][i] / (1 - pow(opt->beta2, opt->t));
        layer->biases[i] -= lr_t * m_hat / (sqrt(v_hat) + opt->epsilon);

        for (int j = 0; j < layer->n_inputs; j++) {
            // 权重衰减
            layer->weights[i][j] *= (1.0 - opt->learning_rate * opt->weight_decay);

            g = layer->weight_grads[i][j];
            opt->m_weights[layer_idx][i][j] = opt->beta1 * opt->m_weights[layer_idx][i][j]
                                              + (1 - opt->beta1) * g;
            opt->v_weights[layer_idx][i][j] = opt->beta2 * opt->v_weights[layer_idx][i][j]
                                              + (1 - opt->beta2) * g * g;
            m_hat = opt->m_weights[layer_idx][i][j] / (1 - pow(opt->beta1, opt->t));
            v_hat = opt->v_weights[layer_idx][i][j] / (1 - pow(opt->beta2, opt->t));
            layer->weights[i][j] -= lr_t * m_hat / (sqrt(v_hat) + opt->epsilon);
        }
    }
}
