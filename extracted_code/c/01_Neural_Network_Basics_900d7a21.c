/*
 * Auto-generated from: 11_Machine_Learning_C\01_Neural_Network_Basics.md
 * Line: 1261
 * Language: c
 * Block ID: 900d7a21
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * MNIST数字识别 - MLP实现
 * 网络结构: 784 -> 256 -> 128 -> 10
 *
 * 编译: gcc -O3 -o mnist mnist.c -lm
 * 运行: ./mnist
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <string.h>

// 字节序转换（MNIST文件为大端）
int32_t swap_endian(int32_t val) {
    return ((val & 0xFF) << 24) | ((val & 0xFF00) << 8) |
           ((val & 0xFF0000) >> 8) | ((val >> 24) & 0xFF);
}

// 读取MNIST图像文件
double** load_mnist_images(const char* filename, int* n_images, int* n_rows, int* n_cols) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        perror("Failed to open image file");
        return NULL;
    }

    // 读取头部
    int32_t magic, num, rows, cols;
    fread(&magic, sizeof(int32_t), 1, fp);
    fread(&num, sizeof(int32_t), 1, fp);
    fread(&rows, sizeof(int32_t), 1, fp);
    fread(&cols, sizeof(int32_t), 1, fp);

    magic = swap_endian(magic);
    num = swap_endian(num);
    rows = swap_endian(rows);
    cols = swap_endian(cols);

    printf("MNIST Images: %d samples, %dx%d pixels\n", num, rows, cols);

    *n_images = num;
    *n_rows = rows;
    *n_cols = cols;

    // 读取图像数据并归一化
    double** images = malloc(num * sizeof(double*));
    uint8_t* buffer = malloc(rows * cols * sizeof(uint8_t));

    for (int i = 0; i < num; i++) {
        images[i] = malloc(rows * cols * sizeof(double));
        fread(buffer, sizeof(uint8_t), rows * cols, fp);

        // 归一化到 [0, 1] 并应用简单的输入标准化
        for (int j = 0; j < rows * cols; j++) {
            images[i][j] = buffer[j] / 255.0;
        }
    }

    free(buffer);
    fclose(fp);
    return images;
}

// 读取MNIST标签文件
int** load_mnist_labels(const char* filename, int* n_labels) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        perror("Failed to open label file");
        return NULL;
    }

    int32_t magic, num;
    fread(&magic, sizeof(int32_t), 1, fp);
    fread(&num, sizeof(int32_t), 1, fp);
    magic = swap_endian(magic);
    num = swap_endian(num);

    printf("MNIST Labels: %d samples\n", num);
    *n_labels = num;

    // 转换为one-hot编码
    int** labels = malloc(num * sizeof(int*));
    uint8_t* buffer = malloc(num * sizeof(uint8_t));
    fread(buffer, sizeof(uint8_t), num, fp);

    for (int i = 0; i < num; i++) {
        labels[i] = calloc(10, sizeof(int));
        labels[i][buffer[i]] = 1;  // one-hot
    }

    free(buffer);
    fclose(fp);
    return labels;
}

// 主训练流程
int main() {
    srand(time(NULL));

    // ============ 加载数据 ============
    int n_train, n_test, rows, cols;

    printf("Loading MNIST dataset...\n");
    double** train_images = load_mnist_images("train-images-idx3-ubyte", &n_train, &rows, &cols);
    int** train_labels = load_mnist_labels("train-labels-idx1-ubyte", &n_train);
    double** test_images = load_mnist_images("t10k-images-idx3-ubyte", &n_test, &rows, &cols);
    int** test_labels = load_mnist_labels("t10k-labels-idx1-ubyte", &n_test);

    if (!train_images || !train_labels || !test_images || !test_labels) {
        printf("\n注意: 需要下载MNIST数据集文件到当前目录:\n");
        printf("  - train-images-idx3-ubyte\n");
        printf("  - train-labels-idx1-ubyte\n");
        printf("  - t10k-images-idx3-ubyte\n");
        printf("  - t10k-labels-idx1-ubyte\n");
        printf("\n下载地址: http://yann.lecun.com/exdb/mnist/\n");
        return 1;
    }

    int input_size = rows * cols;  // 784
    int n_classes = 10;

    // ============ 创建网络 ============
    // 网络结构: 784 -> 256 -> 128 -> 10
    int layer_sizes[] = {input_size, 256, 128, n_classes};
    ActivationType acts[] = {ACT_RELU, ACT_RELU, ACT_SOFTMAX};

    NeuralNetwork* nn = nn_create(layer_sizes, 4, acts, LOSS_CROSS_ENTROPY);

    // 配置Adam优化器
    Optimizer opt = {
        .type = OPT_ADAM,
        .learning_rate = 0.001,
        .beta1 = 0.9,
        .beta2 = 0.999,
        .epsilon = 1e-8,
        .t = 0
    };
    nn->optimizer = &opt;

    // 分配优化器状态
    opt.m_weights = malloc(3 * sizeof(double*));
    opt.v_weights = malloc(3 * sizeof(double*));
    opt.m_biases = malloc(3 * sizeof(double*));
    opt.v_biases = malloc(3 * sizeof(double*));

    for (int i = 0; i < 3; i++) {
        opt.m_weights[i] = calloc(layer_sizes[i] * layer_sizes[i+1], sizeof(double));
        opt.v_weights[i] = calloc(layer_sizes[i] * layer_sizes[i+1], sizeof(double));
        opt.m_biases[i] = calloc(layer_sizes[i+1], sizeof(double));
        opt.v_biases[i] = calloc(layer_sizes[i+1], sizeof(double));
    }

    // ============ 训练 ============
    int n_epochs = 10;
    int batch_size = 64;
    int n_batches = n_train / batch_size;

    printf("\nTraining...\n");
    clock_t start = clock();

    for (int epoch = 0; epoch < n_epochs; epoch++) {
        double epoch_loss = 0.0;

        // 随机打乱
        int* indices = malloc(n_train * sizeof(int));
        for (int i = 0; i < n_train; i++) indices[i] = i;
        for (int i = n_train - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            int tmp = indices[i]; indices[i] = indices[j]; indices[j] = tmp;
        }

        for (int b = 0; b < n_batches; b++) {
            // 清零梯度
            for (int l = 0; l < nn->n_layers; l++) {
                dense_layer_zero_grad(nn->layers[l]);
            }

            double batch_loss = 0.0;

            // 累积批次梯度
            for (int i = 0; i < batch_size; i++) {
                int idx = indices[b * batch_size + i];

                // 将int标签转换为double
                double target[10];
                for (int j = 0; j < 10; j++) target[j] = train_labels[idx][j];

                // 前向传播
                double output[10];
                nn_forward(nn, train_images[idx], output);

                // 计算损失
                double grad[10];
                batch_loss += nn_compute_loss(nn, target, output, grad);

                // 反向传播
                nn_backward(nn, grad);
            }

            // 平均梯度
            for (int l = 0; l < nn->n_layers; l++) {
                DenseLayer* layer = nn->layers[l];
                for (int i = 0; i < layer->n_outputs * layer->n_inputs; i++) {
                    layer->grad_weights->data[i] /= batch_size;
                }
                for (int i = 0; i < layer->n_outputs; i++) {
                    layer->grad_biases[i] /= batch_size;
                }
            }

            // 更新参数
            for (int l = 0; l < nn->n_layers; l++) {
                optimizer_step_adam(&opt, nn->layers[l], l);
            }

            epoch_loss += batch_loss / batch_size;

            // 进度显示
            if ((b + 1) % 100 == 0) {
                printf("\rEpoch %d/%d - Batch %d/%d - Loss: %.4f",
                       epoch + 1, n_epochs, b + 1, n_batches,
                       epoch_loss / (b + 1));
                fflush(stdout);
            }
        }

        free(indices);

        // 每个epoch结束评估
        int correct = 0;
        for (int i = 0; i < n_test; i++) {
            double output[10];
            nn_forward(nn, test_images[i], output);

            int pred = 0;
            for (int j = 1; j < 10; j++) {
                if (output[j] > output[pred]) pred = j;
            }

            int true_label = 0;
            for (int j = 0; j < 10; j++) {
                if (test_labels[i][j] == 1) {
                    true_label = j;
                    break;
                }
            }

            if (pred == true_label) correct++;
        }

        float acc = 100.0f * correct / n_test;
        printf("\rEpoch %d/%d - Loss: %.4f - Test Acc: %.2f%%\n",
               epoch + 1, n_epochs, epoch_loss / n_batches, acc);
    }

    clock_t end = clock();
    double cpu_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\nTraining completed in %.2f seconds\n", cpu_time);

    // ============ 清理 ============
    nn_free(nn);
    for (int i = 0; i < n_train; i++) {
        free(train_images[i]);
        free(train_labels[i]);
    }
    for (int i = 0; i < n_test; i++) {
        free(test_images[i]);
        free(test_labels[i]);
    }
    free(train_images);
    free(train_labels);
    free(test_images);
    free(test_labels);

    return 0;
}
