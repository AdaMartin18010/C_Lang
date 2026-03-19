/*
 * Auto-generated from: 11_Machine_Learning_C\01_Neural_Network_Basics.md
 * Line: 1990
 * Language: c
 * Block ID: 1c9fb5d0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 梯度裁剪: 如果梯度的L2范数超过阈值，则进行缩放
void clip_gradients(NeuralNetwork* nn, double max_norm) {
    // 计算所有参数梯度的L2范数
    double global_norm = 0.0;

    for (int l = 0; l < nn->n_layers; l++) {
        DenseLayer* layer = nn->layers[l];
        for (int i = 0; i < layer->n_outputs; i++) {
            for (int j = 0; j < layer->n_inputs; j++) {
                double g = layer->grad_weights->data[i * layer->n_inputs + j];
                global_norm += g * g;
            }
            global_norm += layer->grad_biases[i] * layer->grad_biases[i];
        }
    }

    global_norm = sqrt(global_norm);

    // 如果超过阈值，进行缩放
    if (global_norm > max_norm) {
        double scale = max_norm / global_norm;
        for (int l = 0; l < nn->n_layers; l++) {
            DenseLayer* layer = nn->layers[l];
            for (int i = 0; i < layer->n_outputs; i++) {
                for (int j = 0; j < layer->n_inputs; j++) {
                    layer->grad_weights->data[i * layer->n_inputs + j] *= scale;
                }
                layer->grad_biases[i] *= scale;
            }
        }
    }
}

// 梯度检查（调试梯度计算正确性）
void gradient_check(NeuralNetwork* nn, const double* input,
                    const double* target, double epsilon) {
    // 数值梯度
    double numerical_grad[1024];
    // 解析梯度（反向传播得到）
    double analytic_grad[1024];

    // 对每个参数计算数值梯度: [f(θ+ε) - f(θ-ε)] / 2ε
    for (int l = 0; l < nn->n_layers; l++) {
        DenseLayer* layer = nn->layers[l];
        for (int i = 0; i < layer->n_outputs; i++) {
            for (int j = 0; j < layer->n_inputs; j++) {
                // 保存原始值
                double orig = layer->weights->data[i * layer->n_inputs + j];

                // f(θ+ε)
                layer->weights->data[i * layer->n_inputs + j] = orig + epsilon;
                double output_plus[256];
                nn_forward(nn, input, output_plus);
                double loss_plus = nn_compute_loss(nn, target, output_plus, NULL);

                // f(θ-ε)
                layer->weights->data[i * layer->n_inputs + j] = orig - epsilon;
                double output_minus[256];
                nn_forward(nn, input, output_minus);
                double loss_minus = nn_compute_loss(nn, target, output_minus, NULL);

                // 数值梯度
                numerical_grad[i * layer->n_inputs + j] =
                    (loss_plus - loss_minus) / (2 * epsilon);

                // 恢复
                layer->weights->data[i * layer->n_inputs + j] = orig;
            }
        }
    }

    // 反向传播得到解析梯度
    dense_layer_zero_grad(layer);
    double output[256];
    nn_forward(nn, input, output);
    double grad_output[256];
    nn_compute_loss(nn, target, output, grad_output);
    nn_backward(nn, grad_output);

    // 比较数值梯度和解析梯度
    double max_diff = 0.0;
    for (int l = 0; l < nn->n_layers; l++) {
        DenseLayer* layer = nn->layers[l];
        for (int i = 0; i < layer->n_outputs; i++) {
            for (int j = 0; j < layer->n_inputs; j++) {
                double diff = fabs(numerical_grad[i * layer->n_inputs + j] -
                                   layer->grad_weights->data[i * layer->n_inputs + j]);
                if (diff > max_diff) max_diff = diff;
            }
        }
    }

    printf("Gradient check - max difference: %e\n", max_diff);
    if (max_diff < 1e-6) {
        printf("✓ Gradients are correct!\n");
    } else {
        printf("✗ Gradients may have errors!\n");
    }
}
