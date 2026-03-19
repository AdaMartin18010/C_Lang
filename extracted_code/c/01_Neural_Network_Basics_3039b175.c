/*
 * Auto-generated from: 11_Machine_Learning_C\01_Neural_Network_Basics.md
 * Line: 1725
 * Language: c
 * Block ID: 3039b175
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifdef _OPENMP
#include <omp.h>
#endif

// OpenMP并行训练
void nn_train_parallel(NeuralNetwork* nn,
                       double** inputs, double** targets,
                       int n_samples, int n_epochs, int batch_size) {
    #ifdef _OPENMP
    int n_threads = omp_get_max_threads();
    printf("Using %d threads\n", n_threads);

    // 为每个线程分配独立的梯度缓冲区
    DenseLayer*** thread_grads = malloc(n_threads * sizeof(DenseLayer**));
    for (int t = 0; t < n_threads; t++) {
        thread_grads[t] = malloc(nn->n_layers * sizeof(DenseLayer*));
        for (int l = 0; l < nn->n_layers; l++) {
            // 创建梯度副本结构
            thread_grads[t][l] = calloc(1, sizeof(DenseLayer));
            thread_grads[t][l]->grad_weights = matrix_create(
                nn->layers[l]->n_outputs, nn->layers[l]->n_inputs);
            thread_grads[t][l]->grad_biases = calloc(
                nn->layers[l]->n_outputs, sizeof(double));
        }
    }

    for (int epoch = 0; epoch < n_epochs; epoch++) {
        double epoch_loss = 0.0;

        #pragma omp parallel for reduction(+:epoch_loss) schedule(dynamic)
        for (int i = 0; i < n_samples; i++) {
            int tid = omp_get_thread_num();

            // 每个线程使用独立的网络副本进行前向/反向
            // 实际实现需要线程本地存储或更复杂的同步

            // 这里简化为共享网络的读操作，梯度累加需要同步
            double output[256];
            nn_forward(nn, inputs[i], output);

            double target_d[256];
            for (int j = 0; j < nn->layers[nn->n_layers-1]->n_outputs; j++) {
                target_d[j] = targets[i][j];
            }

            double grad[256];
            double loss = nn_compute_loss(nn, target_d, output, grad);
            epoch_loss += loss;
        }

        printf("Epoch %d - Loss: %.4f\n", epoch, epoch_loss / n_samples);
    }

    // 释放线程梯度缓冲区
    for (int t = 0; t < n_threads; t++) {
        for (int l = 0; l < nn->n_layers; l++) {
            matrix_free(thread_grads[t][l]->grad_weights);
            free(thread_grads[t][l]->grad_biases);
            free(thread_grads[t][l]);
        }
        free(thread_grads[t]);
    }
    free(thread_grads);
    #endif
}

// 数据并行: 批次内样本并行处理
void compute_batch_gradients_parallel(NeuralNetwork* nn,
                                      double** batch_inputs,
                                      double** batch_targets,
                                      int batch_size,
                                      DenseLayer*** thread_grads) {
    #pragma omp parallel for
    for (int i = 0; i < batch_size; i++) {
        int tid = omp_get_thread_num();

        // 前向传播
        double* temp_output[8];  // 临时缓冲区链
        int max_layer_size = 0;
        for (int l = 0; l < nn->n_layers; l++) {
            if (nn->layers[l]->n_outputs > max_layer_size) {
                max_layer_size = nn->layers[l]->n_outputs;
            }
        }

        double buffer1[1024], buffer2[1024];
        double* current = batch_inputs[i];
        double* next = buffer1;

        for (int l = 0; l < nn->n_layers; l++) {
            dense_layer_forward(nn->layers[l], current, next);
            current = next;
            next = (next == buffer1) ? buffer2 : buffer1;
        }

        // 计算输出梯度
        double grad_output[256];
        nn_compute_loss(nn, batch_targets[i], current, grad_output);

        // 反向传播并累积到线程本地梯度
        // ... 实现略
    }

    // 主线程合并所有线程的梯度
    #pragma omp single
    {
        int n_threads = omp_get_max_threads();
        for (int l = 0; l < nn->n_layers; l++) {
            dense_layer_zero_grad(nn->layers[l]);

            for (int t = 0; t < n_threads; t++) {
                for (int i = 0; i < nn->layers[l]->n_outputs; i++) {
                    for (int j = 0; j < nn->layers[l]->n_inputs; j++) {
                        nn->layers[l]->grad_weights->data[i * nn->layers[l]->n_inputs + j] +=
                            thread_grads[t][l]->grad_weights->data[i * nn->layers[l]->n_inputs + j];
                    }
                    nn->layers[l]->grad_biases[i] += thread_grads[t][l]->grad_biases[i];
                }
            }

            // 平均
            for (int i = 0; i < nn->layers[l]->n_outputs * nn->layers[l]->n_inputs; i++) {
                nn->layers[l]->grad_weights->data[i] /= batch_size;
            }
            for (int i = 0; i < nn->layers[l]->n_outputs; i++) {
                nn->layers[l]->grad_biases[i] /= batch_size;
            }
        }
    }
}
