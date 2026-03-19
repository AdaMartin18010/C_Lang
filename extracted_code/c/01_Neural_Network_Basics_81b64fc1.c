/*
 * Auto-generated from: 11_Machine_Learning_C\01_Neural_Network_Basics.md
 * Line: 2249
 * Language: c
 * Block ID: 81b64fc1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// L2正则化 (权重衰减)
double l2_regularization(NeuralNetwork* nn, double lambda) {
    double reg_loss = 0.0;
    for (int l = 0; l < nn->n_layers; l++) {
        DenseLayer* layer = nn->layers[l];
        for (int i = 0; i < layer->n_outputs * layer->n_inputs; i++) {
            double w = layer->weights->data[i];
            reg_loss += w * w;
            // 将L2梯度加到权重梯度
            layer->grad_weights->data[i] += 2 * lambda * w;
        }
    }
    return lambda * reg_loss;
}

// Dropout层
typedef struct {
    double rate;        // 丢弃概率 (e.g., 0.5)
    int* mask;          // 保留掩码
    int size;
    int is_training;    // 训练/推理模式
} Dropout;

Dropout* dropout_create(double rate, int size) {
    Dropout* d = malloc(sizeof(Dropout));
    d->rate = rate;
    d->size = size;
    d->mask = malloc(size * sizeof(int));
    d->is_training = 1;
    return d;
}

void dropout_forward(Dropout* d, double* data) {
    if (!d->is_training) {
        // 推理时缩放
        for (int i = 0; i < d->size; i++) {
            data[i] *= (1.0 - d->rate);
        }
        return;
    }

    // 训练时随机丢弃
    for (int i = 0; i < d->size; i++) {
        if ((double)rand() / RAND_MAX < d->rate) {
            d->mask[i] = 0;
            data[i] = 0;
        } else {
            d->mask[i] = 1;
            data[i] /= (1.0 - d->rate);  // 反向缩放
        }
    }
}

void dropout_backward(Dropout* d, double* grad) {
    if (!d->is_training) return;

    for (int i = 0; i < d->size; i++) {
        if (d->mask[i] == 0) {
            grad[i] = 0;
        }
    }
}

// 早停 (Early Stopping)
typedef struct {
    double best_val_loss;
    int patience;
    int wait;
    int best_epoch;
    NeuralNetwork* best_model;  // 保存最佳模型
} EarlyStopping;

EarlyStopping* early_stopping_create(int patience) {
    EarlyStopping* es = malloc(sizeof(EarlyStopping));
    es->patience = patience;
    es->wait = 0;
    es->best_val_loss = INFINITY;
    es->best_epoch = 0;
    return es;
}

int early_stopping_check(EarlyStopping* es, double val_loss, int epoch,
                         NeuralNetwork* current_model) {
    if (val_loss < es->best_val_loss) {
        es->best_val_loss = val_loss;
        es->wait = 0;
        es->best_epoch = epoch;
        // 保存当前模型为最佳模型
        // ... 实现模型保存逻辑
        return 0;  // 继续训练
    } else {
        es->wait++;
        if (es->wait >= es->patience) {
            printf("Early stopping at epoch %d (best: %d, loss: %.4f)\n",
                   epoch, es->best_epoch, es->best_val_loss);
            return 1;  // 停止训练
        }
        return 0;
    }
}
