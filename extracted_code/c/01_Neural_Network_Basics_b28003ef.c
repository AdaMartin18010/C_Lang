/*
 * Auto-generated from: 11_Machine_Learning_C\01_Neural_Network_Basics.md
 * Line: 992
 * Language: c
 * Block ID: b28003ef
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 神经网络结构
typedef struct {
    int n_layers;           // 层数（不包括输入层）
    DenseLayer** layers;    // 层数组
    LossType loss_type;
    Optimizer* optimizer;

    // 临时缓冲区（避免重复分配）
    double** grad_buffers;  // [n_layers][max_layer_size]
} NeuralNetwork;

// 创建神经网络
NeuralNetwork* nn_create(int* layer_sizes, int n_layers,
                         ActivationType* activations,
                         LossType loss_type) {
    NeuralNetwork* nn = calloc(1, sizeof(NeuralNetwork));
    nn->n_layers = n_layers - 1;  // 输入层不计入
    nn->loss_type = loss_type;

    // 创建各层
    nn->layers = malloc(nn->n_layers * sizeof(DenseLayer*));
    for (int i = 0; i < nn->n_layers; i++) {
        nn->layers[i] = dense_layer_create(layer_sizes[i],
                                            layer_sizes[i+1],
                                            activations[i]);
    }

    // 分配梯度缓冲区
    int max_size = 0;
    for (int i = 0; i < n_layers; i++) {
        if (layer_sizes[i] > max_size) max_size = layer_sizes[i];
    }
    nn->grad_buffers = malloc(nn->n_layers * sizeof(double*));
    for (int i = 0; i < nn->n_layers; i++) {
        nn->grad_buffers[i] = malloc(max_size * sizeof(double));
    }

    return nn;
}

// 前向传播（完整网络）
void nn_forward(NeuralNetwork* nn, const double* input, double* output) {
    double* current = (double*)input;  // 当前层输入
    double temp_buffer[1024];           // 临时缓冲区

    for (int i = 0; i < nn->n_layers; i++) {
        double* next = (i == nn->n_layers - 1) ? output : temp_buffer;
        dense_layer_forward(nn->layers[i], current, next);
        current = next;
    }
}

// 计算损失
double nn_compute_loss(NeuralNetwork* nn, const double* y_true,
                       const double* y_pred, double* grad_output) {
    int output_size = nn->layers[nn->n_layers - 1]->n_outputs;

    switch (nn->loss_type) {
        case LOSS_MSE:
            if (grad_output) {
                loss_mse_grad(y_true, y_pred, output_size, grad_output);
            }
            return loss_mse(y_true, y_pred, output_size);

        case LOSS_CROSS_ENTROPY:
            if (grad_output) {
                // Softmax + CrossEntropy 梯度简化
                for (int i = 0; i < output_size; i++) {
                    grad_output[i] = y_pred[i] - y_true[i];
                }
            }
            return loss_cross_entropy(y_true, y_pred, output_size);

        case LOSS_BINARY_CROSS_ENTROPY:
            if (grad_output) {
                const double eps = 1e-7;
                for (int i = 0; i < output_size; i++) {
                    grad_output[i] = -(y_true[i] / fmax(y_pred[i], eps)
                                       - (1-y_true[i]) / fmax(1-y_pred[i], eps));
                }
            }
            // 计算损失
            double loss = 0.0;
            const double eps = 1e-7;
            for (int i = 0; i < output_size; i++) {
                loss -= y_true[i] * log(fmax(y_pred[i], eps))
                        + (1-y_true[i]) * log(fmax(1-y_pred[i], eps));
            }
            return loss;

        default:
            return 0.0;
    }
}

// 反向传播（单样本）
void nn_backward(NeuralNetwork* nn, const double* grad_output) {
    // 从输出层开始反向传播
    double* current_grad = (double*)grad_output;

    for (int i = nn->n_layers - 1; i >= 0; i--) {
        double* prev_grad = (i == 0) ? NULL : nn->grad_buffers[i-1];
        dense_layer_backward(nn->layers[i], current_grad, prev_grad);
        current_grad = prev_grad;
    }
}

// 训练单步（前向+反向+更新）
double nn_train_step(NeuralNetwork* nn, const double* input,
                     const double* target) {
    // 清零梯度
    for (int i = 0; i < nn->n_layers; i++) {
        dense_layer_zero_grad(nn->layers[i]);
    }

    // 前向传播
    double output[256];  // 假设最大输出维度
    nn_forward(nn, input, output);

    // 计算损失和输出层梯度
    double grad_output[256];
    double loss = nn_compute_loss(nn, target, output, grad_output);

    // 反向传播
    nn_backward(nn, grad_output);

    // 参数更新
    if (nn->optimizer) {
        for (int i = 0; i < nn->n_layers; i++) {
            switch (nn->optimizer->type) {
                case OPT_SGD:
                    optimizer_step_sgd(nn->optimizer, nn->layers[i]);
                    break;
                case OPT_MOMENTUM:
                    optimizer_step_momentum(nn->optimizer, nn->layers[i], i);
                    break;
                case OPT_ADAM:
                    optimizer_step_adam(nn->optimizer, nn->layers[i], i);
                    break;
                case OPT_ADAMW:
                    optimizer_step_adamw(nn->optimizer, nn->layers[i], i);
                    break;
            }
        }
    }

    return loss;
}

// 批量训练
void nn_train_epoch(NeuralNetwork* nn,
                    double** inputs, double** targets, int n_samples,
                    int batch_size) {
    // 随机打乱索引
    int* indices = malloc(n_samples * sizeof(int));
    for (int i = 0; i < n_samples; i++) indices[i] = i;
    for (int i = n_samples - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = indices[i]; indices[i] = indices[j]; indices[j] = tmp;
    }

    // 按批次训练
    int n_batches = (n_samples + batch_size - 1) / batch_size;
    for (int b = 0; b < n_batches; b++) {
        int start = b * batch_size;
        int end = (start + batch_size < n_samples) ? start + batch_size : n_samples;

        // 累积梯度
        for (int i = 0; i < nn->n_layers; i++) {
            dense_layer_zero_grad(nn->layers[i]);
        }

        double batch_loss = 0.0;
        for (int i = start; i < end; i++) {
            int idx = indices[i];

            // 前向传播
            double output[256];
            nn_forward(nn, inputs[idx], output);

            // 计算损失
            double grad_output[256];
            batch_loss += nn_compute_loss(nn, targets[idx], output, grad_output);

            // 反向传播（累积梯度）
            nn_backward(nn, grad_output);
        }

        // 平均梯度
        int actual_batch_size = end - start;
        for (int l = 0; l < nn->n_layers; l++) {
            DenseLayer* layer = nn->layers[l];
            for (int i = 0; i < layer->n_outputs * layer->n_inputs; i++) {
                layer->grad_weights->data[i] /= actual_batch_size;
            }
            for (int i = 0; i < layer->n_outputs; i++) {
                layer->grad_biases[i] /= actual_batch_size;
            }
        }

        // 更新参数
        for (int i = 0; i < nn->n_layers; i++) {
            optimizer_step_adam(nn->optimizer, nn->layers[i], i);
        }
    }

    free(indices);
}

// 预测
double* nn_predict(NeuralNetwork* nn, const double* input, double* output) {
    nn_forward(nn, input, output);
    return output;
}

// 评估准确率
float nn_evaluate_accuracy(NeuralNetwork* nn,
                           double** inputs, int** targets,
                           int n_samples, int n_classes) {
    int correct = 0;
    double* output = malloc(n_classes * sizeof(double));

    for (int i = 0; i < n_samples; i++) {
        nn_predict(nn, inputs[i], output);

        // 找到预测类别（最大概率）
        int pred_class = 0;
        double max_prob = output[0];
        for (int j = 1; j < n_classes; j++) {
            if (output[j] > max_prob) {
                max_prob = output[j];
                pred_class = j;
            }
        }

        // 找到真实类别
        int true_class = 0;
        for (int j = 0; j < n_classes; j++) {
            if (targets[i][j] == 1) {
                true_class = j;
                break;
            }
        }

        if (pred_class == true_class) correct++;
    }

    free(output);
    return (float)correct / n_samples;
}

// 释放网络
void nn_free(NeuralNetwork* nn) {
    if (!nn) return;
    for (int i = 0; i < nn->n_layers; i++) {
        dense_layer_free(nn->layers[i]);
    }
    free(nn->layers);
    for (int i = 0; i < nn->n_layers; i++) {
        free(nn->grad_buffers[i]);
    }
    free(nn->grad_buffers);
    free(nn);
}
