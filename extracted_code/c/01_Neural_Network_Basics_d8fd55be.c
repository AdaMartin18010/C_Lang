/*
 * Auto-generated from: 11_Machine_Learning_C\01_Neural_Network_Basics.md
 * Line: 273
 * Language: c
 * Block ID: d8fd55be
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 损失函数类型
typedef enum {
    LOSS_MSE,
    LOSS_CROSS_ENTROPY,
    LOSS_BINARY_CROSS_ENTROPY,
    LOSS_HUBER
} LossType;

// MSE损失
double loss_mse(const double* y_true, const double* y_pred, int n) {
    double loss = 0.0;
    for (int i = 0; i < n; i++) {
        double diff = y_true[i] - y_pred[i];
        loss += diff * diff;
    }
    return loss / n;
}

// MSE梯度: ∂L/∂ŷ = -2(y - ŷ)/n
double* loss_mse_grad(const double* y_true, const double* y_pred,
                      int n, double* grad) {
    for (int i = 0; i < n; i++) {
        grad[i] = -2.0 * (y_true[i] - y_pred[i]) / n;
    }
    return grad;
}

// Softmax + 交叉熵（数值稳定版本）
void softmax(const double* z, double* y, int n) {
    // 减去最大值防止溢出
    double max_z = z[0];
    for (int i = 1; i < n; i++) {
        if (z[i] > max_z) max_z = z[i];
    }

    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        y[i] = exp(z[i] - max_z);
        sum += y[i];
    }
    for (int i = 0; i < n; i++) {
        y[i] /= sum;
    }
}

// 交叉熵损失
double loss_cross_entropy(const double* y_true, const double* y_pred, int n) {
    double loss = 0.0;
    const double epsilon = 1e-15;  // 防止log(0)
    for (int i = 0; i < n; i++) {
        loss -= y_true[i] * log(fmax(y_pred[i], epsilon));
    }
    return loss;
}

// 交叉熵 + Softmax的梯度（简化形式）
double* loss_softmax_cross_entropy_grad(const double* y_true,
                                        const double* y_pred,
                                        int n, double* grad) {
    for (int i = 0; i < n; i++) {
        grad[i] = y_pred[i] - y_true[i];  // ŷ - y
    }
    return grad;
}
