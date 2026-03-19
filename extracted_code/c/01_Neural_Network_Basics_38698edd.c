/*
 * Auto-generated from: 11_Machine_Learning_C\01_Neural_Network_Basics.md
 * Line: 845
 * Language: c
 * Block ID: 38698edd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 神经网络层
typedef struct {
    int n_inputs;
    int n_outputs;

    // 参数
    Matrix* weights;    // [n_outputs x n_inputs]
    double* biases;     // [n_outputs]

    // 梯度
    Matrix* grad_weights;  // [n_outputs x n_inputs]
    double* grad_biases;   // [n_outputs]

    // 缓存（用于反向传播）
    double* input_cache;   // [n_inputs]
    double* z_cache;       // [n_outputs] 线性输出
    double* output_cache;  // [n_outputs]

    // 激活函数
    ActivationType act_type;
    double (*activation)(double);
    double (*activation_deriv)(double);
} DenseLayer;

// 创建全连接层
DenseLayer* dense_layer_create(int n_inputs, int n_outputs, ActivationType act) {
    DenseLayer* layer = calloc(1, sizeof(DenseLayer));
    layer->n_inputs = n_inputs;
    layer->n_outputs = n_outputs;
    layer->act_type = act;

    // 选择初始化方式和激活函数
    InitType init_type;
    switch (act) {
        case ACT_RELU:
        case ACT_LEAKY_RELU:
        case ACT_ELU:
            layer->activation = relu;
            layer->activation_deriv = relu_deriv;
            init_type = INIT_HE;
            break;
        case ACT_TANH:
            layer->activation = tanh_act;
            layer->activation_deriv = tanh_deriv;
            init_type = INIT_XAVIER;
            break;
        case ACT_SIGMOID:
            layer->activation = sigmoid;
            layer->activation_deriv = sigmoid_deriv;
            init_type = INIT_XAVIER;
            break;
        default:
            layer->activation = relu;
            layer->activation_deriv = relu_deriv;
            init_type = INIT_HE;
    }

    // 分配参数
    layer->weights = matrix_create(n_outputs, n_inputs);
    layer->biases = calloc(n_outputs, sizeof(double));
    layer->grad_weights = matrix_create(n_outputs, n_inputs);
    layer->grad_biases = calloc(n_outputs, sizeof(double));

    // 初始化权重
    matrix_init_random(layer->weights, init_type);

    // 缓存
    layer->input_cache = malloc(n_inputs * sizeof(double));
    layer->z_cache = malloc(n_outputs * sizeof(double));
    layer->output_cache = malloc(n_outputs * sizeof(double));

    return layer;
}

// 层前向传播
void dense_layer_forward(DenseLayer* layer, const double* input, double* output) {
    // 缓存输入
    memcpy(layer->input_cache, input, layer->n_inputs * sizeof(double));

    // z = W * x + b
    matrix_vector_multiply(layer->weights, input, layer->z_cache);
    for (int i = 0; i < layer->n_outputs; i++) {
        layer->z_cache[i] += layer->biases[i];
        layer->output_cache[i] = layer->activation(layer->z_cache[i]);
        output[i] = layer->output_cache[i];
    }
}

// 层反向传播
// grad_upstream: ∂L/∂a [n_outputs]
// grad_input: ∂L/∂x [n_inputs] (输出给前一层的梯度)
void dense_layer_backward(DenseLayer* layer, const double* grad_upstream,
                          double* grad_input) {
    // δ = grad_upstream ⊙ σ'(z)
    double* delta = malloc(layer->n_outputs * sizeof(double));
    for (int i = 0; i < layer->n_outputs; i++) {
        delta[i] = grad_upstream[i] * layer->activation_deriv(layer->z_cache[i]);
    }

    // ∇W += δ · x^T
    for (int i = 0; i < layer->n_outputs; i++) {
        for (int j = 0; j < layer->n_inputs; j++) {
            layer->grad_weights->data[i * layer->n_inputs + j] +=
                delta[i] * layer->input_cache[j];
        }
        layer->grad_biases[i] += delta[i];
    }

    // ∂L/∂x = W^T · δ
    if (grad_input) {
        for (int j = 0; j < layer->n_inputs; j++) {
            grad_input[j] = 0.0;
            for (int i = 0; i < layer->n_outputs; i++) {
                grad_input[j] += layer->weights->data[i * layer->n_inputs + j] * delta[i];
            }
        }
    }

    free(delta);
}

// 清零梯度
void dense_layer_zero_grad(DenseLayer* layer) {
    memset(layer->grad_weights->data, 0,
           layer->n_outputs * layer->n_inputs * sizeof(double));
    memset(layer->grad_biases, 0, layer->n_outputs * sizeof(double));
}

// 释放层
void dense_layer_free(DenseLayer* layer) {
    if (!layer) return;
    matrix_free(layer->weights);
    matrix_free(layer->grad_weights);
    free(layer->biases);
    free(layer->grad_biases);
    free(layer->input_cache);
    free(layer->z_cache);
    free(layer->output_cache);
    free(layer);
}
