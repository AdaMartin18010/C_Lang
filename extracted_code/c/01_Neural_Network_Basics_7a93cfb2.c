/*
 * Auto-generated from: 11_Machine_Learning_C\01_Neural_Network_Basics.md
 * Line: 2354
 * Language: c
 * Block ID: 7a93cfb2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ==================== 数值稳定性陷阱与解决方案 ====================

/*
 * 问题1: Softmax数值溢出
 * exp(1000) = inf，导致softmax计算失败
 * 解决方案: 减去最大值
 */
void softmax_stable(const double* z, double* y, int n) {
    double max_z = z[0];
    for (int i = 1; i < n; i++) {
        if (z[i] > max_z) max_z = z[i];
    }

    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        y[i] = exp(z[i] - max_z);  // 减去最大值，结果不变但数值稳定
        sum += y[i];
    }

    for (int i = 0; i < n; i++) {
        y[i] /= sum;
    }
}

/*
 * 问题2: log(0) = -inf，交叉熵损失计算失败
 * 解决方案: 添加epsilon裁剪
 */
double cross_entropy_stable(const double* y_true, const double* y_pred, int n) {
    const double epsilon = 1e-15;
    double loss = 0.0;
    for (int i = 0; i < n; i++) {
        // 裁剪预测值到[epsilon, 1-epsilon]
        double p = fmax(fmin(y_pred[i], 1.0 - epsilon), epsilon);
        loss -= y_true[i] * log(p);
    }
    return loss;
}

/*
 * 问题3: 浮点数累加精度丢失
 * 解决方案: Kahan求和算法
 */
double kahan_sum(const double* arr, int n) {
    double sum = 0.0;
    double c = 0.0;  // 补偿值

    for (int i = 0; i < n; i++) {
        double y = arr[i] - c;
        double t = sum + y;
        c = (t - sum) - y;
        sum = t;
    }

    return sum;
}

/*
 * 问题4: 除以零
 * 解决方案: 添加保护
 */
double safe_divide(double a, double b, double default_val) {
    if (fabs(b) < 1e-15) {
        return default_val;
    }
    return a / b;
}

/*
 * 问题5: 平方根内负数
 * 解决方案: 最大值保护
 */
double safe_sqrt(double x) {
    return sqrt(fmax(x, 0.0));
}
