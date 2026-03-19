/*
 * Auto-generated from: 11_Machine_Learning_C\01_Neural_Network_Basics.md
 * Line: 436
 * Language: c
 * Block ID: 5cacbafb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ==================== Sigmoid ====================
inline double sigmoid(double x) {
    // 数值稳定版本
    if (x >= 0) {
        return 1.0 / (1.0 + exp(-x));
    } else {
        double exp_x = exp(x);
        return exp_x / (1.0 + exp_x);
    }
}

inline double sigmoid_deriv(double x) {
    double s = sigmoid(x);
    return s * (1.0 - s);
}

// ==================== Tanh ====================
inline double tanh_act(double x) {
    return tanh(x);
}

inline double tanh_deriv(double x) {
    double t = tanh(x);
    return 1.0 - t * t;
}

// ==================== ReLU ====================
inline double relu(double x) {
    return x > 0 ? x : 0;
}

inline double relu_deriv(double x) {
    return x > 0 ? 1.0 : 0.0;
}

// ==================== LeakyReLU ====================
#define LEAKY_RELU_ALPHA 0.01

inline double leaky_relu(double x) {
    return x > 0 ? x : LEAKY_RELU_ALPHA * x;
}

inline double leaky_relu_deriv(double x) {
    return x > 0 ? 1.0 : LEAKY_RELU_ALPHA;
}

// ==================== ELU ====================
#define ELU_ALPHA 1.0

inline double elu(double x) {
    return x > 0 ? x : ELU_ALPHA * (exp(x) - 1.0);
}

inline double elu_deriv(double x) {
    return x > 0 ? 1.0 : ELU_ALPHA * exp(x);
}

// ==================== GELU (现代Transformer常用) ====================
inline double gelu(double x) {
    // 近似: 0.5 * x * (1 + tanh(sqrt(2/π) * (x + 0.044715 * x^3)))
    const double sqrt_2_over_pi = 0.7978845608;
    double cdf = 0.5 * (1.0 + tanh(sqrt_2_over_pi * (x + 0.044715 * x * x * x)));
    return x * cdf;
}

// ==================== Swish/SiLU ====================
inline double silu(double x) {
    return x * sigmoid(x);
}

// 激活函数选择器
typedef struct {
    const char* name;
    double (*func)(double);
    double (*deriv)(double);
    ActivationType type;
} ActivationEntry;

const ActivationEntry ACTIVATIONS[] = {
    {"sigmoid", sigmoid, sigmoid_deriv, ACT_SIGMOID},
    {"tanh", tanh_act, tanh_deriv, ACT_TANH},
    {"relu", relu, relu_deriv, ACT_RELU},
    {"leaky_relu", leaky_relu, leaky_relu_deriv, ACT_LEAKY_RELU},
    {"elu", elu, elu_deriv, ACT_ELU},
    {"gelu", gelu, NULL, ACT_GELU},
    {"silu", silu, NULL, ACT_SILU},
    {NULL, NULL, NULL, ACT_NONE}
};
