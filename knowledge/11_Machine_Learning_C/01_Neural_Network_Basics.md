# C语言神经网络基础

> **目标**: 用C语言实现基础神经网络，理解前向传播和反向传播
> **技术栈**: 纯C实现，依赖基础数学库
> **难度**: L3 | **预估时间**: 3-4小时

---

## 1. 神经网络基础概念

### 1.1 感知机模型

```
输入层          隐藏层          输出层
   x1
    \           h1
     \         /  \
      \       /    \
       o-----o      o----- y
      /       \    /
     /         \  /
    /           h2
   x2

数学表达: y = f(w1*x1 + w2*x2 + b)
```

### 1.2 C语言结构定义

```c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// 激活函数类型
typedef enum {
    ACT_SIGMOID,
    ACT_RELU,
    ACT_TANH,
    ACT_LINEAR
} ActivationType;

// 单层神经网络(感知机)
typedef struct {
    int n_inputs;      // 输入数量
    int n_outputs;     // 输出数量
    double** weights;  // 权重矩阵 [n_outputs][n_inputs]
    double* biases;    // 偏置 [n_outputs]
    ActivationType activation;
} DenseLayer;

// 神经网络
typedef struct {
    int n_layers;
    DenseLayer** layers;
    double learning_rate;
} NeuralNetwork;
```

---

## 2. 激活函数实现

```c
// Sigmoid: f(x) = 1 / (1 + exp(-x))
double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

double sigmoid_derivative(double x) {
    double s = sigmoid(x);
    return s * (1.0 - s);
}

// ReLU: f(x) = max(0, x)
double relu(double x) {
    return x > 0 ? x : 0;
}

double relu_derivative(double x) {
    return x > 0 ? 1 : 0;
}

// Tanh: f(x) = (exp(x) - exp(-x)) / (exp(x) + exp(-x))
double tanh_activation(double x) {
    return tanh(x);
}

double tanh_derivative(double x) {
    double t = tanh(x);
    return 1.0 - t * t;
}

// 激活函数应用
double apply_activation(double x, ActivationType type) {
    switch (type) {
        case ACT_SIGMOID: return sigmoid(x);
        case ACT_RELU: return relu(x);
        case ACT_TANH: return tanh_activation(x);
        case ACT_LINEAR: return x;
        default: return x;
    }
}

double apply_activation_derivative(double x, ActivationType type) {
    switch (type) {
        case ACT_SIGMOID: return sigmoid_derivative(x);
        case ACT_RELU: return relu_derivative(x);
        case ACT_TANH: return tanh_derivative(x);
        case ACT_LINEAR: return 1.0;
        default: return 1.0;
    }
}
```

---

## 3. 层操作实现

```c
// 创建全连接层
DenseLayer* layer_create(int n_inputs, int n_outputs, ActivationType act) {
    DenseLayer* layer = malloc(sizeof(DenseLayer));
    layer->n_inputs = n_inputs;
    layer->n_outputs = n_outputs;
    layer->activation = act;

    // 分配权重矩阵
    layer->weights = malloc(n_outputs * sizeof(double*));
    for (int i = 0; i < n_outputs; i++) {
        layer->weights[i] = malloc(n_inputs * sizeof(double));
        // Xavier初始化
        for (int j = 0; j < n_inputs; j++) {
            layer->weights[i][j] = ((double)rand() / RAND_MAX - 0.5) * 2.0
                                   * sqrt(6.0 / (n_inputs + n_outputs));
        }
    }

    // 初始化偏置为0
    layer->biases = calloc(n_outputs, sizeof(double));

    return layer;
}

// 前向传播
void layer_forward(DenseLayer* layer, const double* input,
                   double* output, double* pre_activation) {
    for (int i = 0; i < layer->n_outputs; i++) {
        double sum = layer->biases[i];
        for (int j = 0; j < layer->n_inputs; j++) {
            sum += layer->weights[i][j] * input[j];
        }
        if (pre_activation) pre_activation[i] = sum;
        output[i] = apply_activation(sum, layer->activation);
    }
}

// 释放层内存
void layer_free(DenseLayer* layer) {
    for (int i = 0; i < layer->n_outputs; i++) {
        free(layer->weights[i]);
    }
    free(layer->weights);
    free(layer->biases);
    free(layer);
}
```

---

## 4. 神经网络完整实现

```c
// 创建神经网络
NeuralNetwork* nn_create(int n_layers, int* layer_sizes,
                         ActivationType* activations, double lr) {
    NeuralNetwork* nn = malloc(sizeof(NeuralNetwork));
    nn->n_layers = n_layers;
    nn->learning_rate = lr;
    nn->layers = malloc((n_layers - 1) * sizeof(DenseLayer*));

    for (int i = 0; i < n_layers - 1; i++) {
        nn->layers[i] = layer_create(layer_sizes[i],
                                      layer_sizes[i+1],
                                      activations[i]);
    }

    return nn;
}

// 前向传播(完整网络)
void nn_forward(NeuralNetwork* nn, const double* input, double* output,
                double** hidden_outputs, double** pre_activations) {
    double current[256];  // 假设最大层大小
    double next[256];

    // 复制输入
    int input_size = nn->layers[0]->n_inputs;
    for (int i = 0; i < input_size; i++) {
        current[i] = input[i];
    }

    // 逐层传播
    for (int i = 0; i < nn->n_layers - 1; i++) {
        layer_forward(nn->layers[i], current, next,
                      pre_activations ? pre_activations[i] : NULL);

        if (hidden_outputs) {
            for (int j = 0; j < nn->layers[i]->n_outputs; j++) {
                hidden_outputs[i][j] = next[j];
            }
        }

        // 准备下一层输入
        for (int j = 0; j < nn->layers[i]->n_outputs; j++) {
            current[j] = next[j];
        }
    }

    // 复制输出
    int output_size = nn->layers[nn->n_layers - 2]->n_outputs;
    for (int i = 0; i < output_size; i++) {
        output[i] = current[i];
    }
}

// 均方误差损失
double mse_loss(const double* predicted, const double* target, int n) {
    double sum = 0;
    for (int i = 0; i < n; i++) {
        double diff = predicted[i] - target[i];
        sum += diff * diff;
    }
    return sum / n;
}

// 反向传播训练
void nn_train_step(NeuralNetwork* nn, const double* input,
                   const double* target) {
    // 存储中间结果
    double hidden[10][256];      // 隐藏层输出
    double pre_act[10][256];     // 激活前值
    double output[256];

    // 前向传播
    nn_forward(nn, input, output, hidden, pre_act);

    int n_layers = nn->n_layers - 1;

    // 计算输出层误差
    double delta[10][256];  // 每层的delta
    int output_size = nn->layers[n_layers - 1]->n_outputs;

    for (int i = 0; i < output_size; i++) {
        double error = output[i] - target[i];
        delta[n_layers - 1][i] = error *
            apply_activation_derivative(pre_act[n_layers - 1][i],
                nn->layers[n_layers - 1]->activation);
    }

    // 反向传播误差
    for (int l = n_layers - 2; l >= 0; l--) {
        int curr_size = nn->layers[l]->n_outputs;
        int next_size = nn->layers[l + 1]->n_outputs;

        for (int i = 0; i < curr_size; i++) {
            double error = 0;
            for (int j = 0; j < next_size; j++) {
                error += nn->layers[l + 1]->weights[j][i] * delta[l + 1][j];
            }
            delta[l][i] = error * apply_activation_derivative(pre_act[l][i],
                nn->layers[l]->activation);
        }
    }

    // 更新权重和偏置
    for (int l = 0; l < n_layers; l++) {
        int n_out = nn->layers[l]->n_outputs;
        int n_in = nn->layers[l]->n_inputs;

        const double* layer_input = (l == 0) ? input : hidden[l - 1];

        for (int i = 0; i < n_out; i++) {
            // 更新偏置
            nn->layers[l]->biases[i] -= nn->learning_rate * delta[l][i];

            // 更新权重
            for (int j = 0; j < n_in; j++) {
                nn->layers[l]->weights[i][j] -= nn->learning_rate *
                                                delta[l][i] * layer_input[j];
            }
        }
    }
}
```

---

## 5. 训练示例：XOR问题

```c
#include <stdio.h>

int main() {
    srand(time(NULL));

    // XOR数据集
    double X[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    double Y[4][1] = {{0}, {1}, {1}, {0}};

    // 网络结构: 2 -> 4 -> 1
    int layer_sizes[] = {2, 4, 1};
    ActivationType acts[] = {ACT_SIGMOID, ACT_SIGMOID};

    NeuralNetwork* nn = nn_create(3, layer_sizes, acts, 0.5);

    // 训练
    printf("Training XOR...\n");
    for (int epoch = 0; epoch < 10000; epoch++) {
        double total_loss = 0;

        for (int i = 0; i < 4; i++) {
            nn_train_step(nn, X[i], Y[i]);

            double pred[1];
            nn_forward(nn, X[i], pred, NULL, NULL);
            total_loss += mse_loss(pred, Y[i], 1);
        }

        if (epoch % 1000 == 0) {
            printf("Epoch %d, Loss: %.6f\n", epoch, total_loss / 4);
        }
    }

    // 测试
    printf("\nTesting:\n");
    for (int i = 0; i < 4; i++) {
        double pred[1];
        nn_forward(nn, X[i], pred, NULL, NULL);
        printf("%.0f XOR %.0f = %.4f (expected: %.0f)\n",
               X[i][0], X[i][1], pred[0], Y[i][0]);
    }

    return 0;
}
```

---

## 6. 性能优化

```c
// 使用BLAS进行矩阵运算加速
#ifdef USE_BLAS
#include <cblas.h>

void layer_forward_blas(DenseLayer* layer, const double* input,
                        double* output) {
    // y = Wx + b
    cblas_dgemv(CblasRowMajor, CblasNoTrans,
                layer->n_outputs, layer->n_inputs,
                1.0, layer->weights[0], layer->n_inputs,
                input, 1, 0.0, output, 1);

    // 添加偏置和激活
    for (int i = 0; i < layer->n_outputs; i++) {
        output[i] = apply_activation(output[i] + layer->biases[i],
                                     layer->activation);
    }
}
#endif

// 批处理训练
void nn_train_batch(NeuralNetwork* nn,
                    const double** inputs,
                    const double** targets,
                    int batch_size) {
    for (int i = 0; i < batch_size; i++) {
        nn_train_step(nn, inputs[i], targets[i]);
    }
}
```

---

## 7. 扩展：保存和加载模型

```c
// 保存模型到文件
void nn_save(NeuralNetwork* nn, const char* filename) {
    FILE* f = fopen(filename, "wb");
    if (!f) return;

    // 写入层数
    fwrite(&nn->n_layers, sizeof(int), 1, f);
    fwrite(&nn->learning_rate, sizeof(double), 1, f);

    // 写入每层参数
    for (int i = 0; i < nn->n_layers - 1; i++) {
        DenseLayer* layer = nn->layers[i];
        fwrite(&layer->n_inputs, sizeof(int), 1, f);
        fwrite(&layer->n_outputs, sizeof(int), 1, f);
        fwrite(&layer->activation, sizeof(ActivationType), 1, f);

        // 写入权重和偏置
        for (int j = 0; j < layer->n_outputs; j++) {
            fwrite(layer->weights[j], sizeof(double), layer->n_inputs, f);
        }
        fwrite(layer->biases, sizeof(double), layer->n_outputs, f);
    }

    fclose(f);
}

// 从文件加载模型
NeuralNetwork* nn_load(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) return NULL;

    int n_layers;
    double lr;
    fread(&n_layers, sizeof(int), 1, f);
    fread(&lr, sizeof(double), 1, f);

    int* sizes = malloc(n_layers * sizeof(int));
    ActivationType* acts = malloc((n_layers - 1) * sizeof(ActivationType));

    // 读取层大小和激活函数
    for (int i = 0; i < n_layers - 1; i++) {
        int n_in, n_out;
        fread(&n_in, sizeof(int), 1, f);
        fread(&n_out, sizeof(int), 1, f);
        fread(&acts[i], sizeof(ActivationType), 1, f);
        sizes[i] = n_in;
        sizes[i + 1] = n_out;
    }

    NeuralNetwork* nn = nn_create(n_layers, sizes, acts, lr);

    // 读取权重和偏置
    for (int i = 0; i < n_layers - 1; i++) {
        DenseLayer* layer = nn->layers[i];
        for (int j = 0; j < layer->n_outputs; j++) {
            fread(layer->weights[j], sizeof(double), layer->n_inputs, f);
        }
        fread(layer->biases, sizeof(double), layer->n_outputs, f);
    }

    free(sizes);
    free(acts);
    fclose(f);

    return nn;
}
```

---

> **核心洞察**: 神经网络的核心是矩阵运算和链式求导。C语言实现虽然代码量较大，但能提供最大控制权和性能优化空间。

---

*最后更新: 2026-03-14*
