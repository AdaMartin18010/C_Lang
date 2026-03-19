/*
 * Auto-generated from: 11_Machine_Learning_C\01_Neural_Network_Basics.md
 * Line: 382
 * Language: c
 * Block ID: 1880d535
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 层的反向传播
// 输入: upstream_grad = ∂L/∂a^{[l]}
// 输出: 返回 ∂L/∂a^{[l-1]} (传给前一层的梯度)
double* layer_backward(Layer* layer, const double* upstream_grad,
                       double* grad_to_input) {
    // δ^{[l]} = upstream_grad ⊙ σ'(z^{[l]})
    double* delta = malloc(layer->n_outputs * sizeof(double));
    for (int i = 0; i < layer->n_outputs; i++) {
        delta[i] = upstream_grad[i] * layer->activate_deriv(layer->z_cache[i]);
    }

    // ∇W^{[l]} = δ^{[l]} · a^{[l-1]T}
    for (int i = 0; i < layer->n_outputs; i++) {
        for (int j = 0; j < layer->n_inputs; j++) {
            layer->weight_grads[i][j] += delta[i] * layer->input_cache[j];
        }
        layer->bias_grads[i] += delta[i];
    }

    // ∂L/∂a^{[l-1]} = W^{[l]T} · δ^{[l]}
    if (grad_to_input) {
        for (int j = 0; j < layer->n_inputs; j++) {
            grad_to_input[j] = 0.0;
            for (int i = 0; i < layer->n_outputs; i++) {
                grad_to_input[j] += layer->weights[i][j] * delta[i];
            }
        }
    }

    free(delta);
    return grad_to_input;
}
