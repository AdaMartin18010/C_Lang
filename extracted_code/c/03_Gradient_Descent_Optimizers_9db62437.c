/*
 * Auto-generated from: 11_Machine_Learning_C\03_Gradient_Descent_Optimizers.md
 * Line: 67
 * Language: c
 * Block ID: 9db62437
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define BATCH_SIZE 32

void minibatch_update(Network* net, Matrix* X_batch, Matrix* y_batch) {
    Matrix* gradients = compute_gradients(net, X_batch, y_batch);

    // 平均梯度
    scale_matrix(gradients, 1.0 / BATCH_SIZE);

    // 更新参数
    for (int i = 0; i < net->n_layers; i++) {
        axpy_matrix(-net->learning_rate, gradients[i], net->weights[i]);
    }

    free_matrix(gradients);
}
