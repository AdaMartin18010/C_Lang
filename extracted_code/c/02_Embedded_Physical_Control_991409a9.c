/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Physical_Control.md
 * Line: 558
 * Language: c
 * Block ID: 991409a9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 定点数: 用整数模拟小数，避免浮点运算
// 格式: Qm.n = m位整数 + n位小数，共m+n+1位（含符号）
// ============================================================

// Q15格式: 1位符号 + 15位小数，范围[-1, 1-2^-15]
typedef int16_t q15_t;
#define Q15_SHIFT   15
#define Q15_ONE     (1 << Q15_SHIFT)  // 1.0 = 32768

// Q31格式: 1位符号 + 31位小数
typedef int32_t q31_t;
#define Q31_SHIFT   31
#define Q31_ONE     (1LL << Q31_SHIFT)

// 浮点转定点
static inline q15_t float_to_q15(float f) {
    return (q15_t)(f * Q15_ONE);
}

// 定点转浮点
static inline float q15_to_float(q15_t q) {
    return (float)q / Q15_ONE;
}

// 定点加法 (直接相加，注意溢出)
static inline q15_t q15_add(q15_t a, q15_t b) {
    int32_t sum = (int32_t)a + (int32_t)b;  // 提升到32位防止溢出
    // 饱和处理
    if (sum > INT16_MAX) return INT16_MAX;
    if (sum < INT16_MIN) return INT16_MIN;
    return (q15_t)sum;
}

// 定点乘法 (需要移位)
// (a * b) >> n，注意中间结果用32位
static inline q15_t q15_mul(q15_t a, q15_t b) {
    int32_t prod = (int32_t)a * (int32_t)b;  // 32位中间结果
    return (q15_t)((prod + (1 << (Q15_SHIFT - 1))) >> Q15_SHIFT);  // 四舍五入
}

// 定点除法
static inline q15_t q15_div(q15_t a, q15_t b) {
    int32_t num = (int32_t)a << Q15_SHIFT;   // 被除数左移
    return (q15_t)(num / b);
}

// 应用: PID控制器 (纯定点实现)
typedef struct {
    q15_t kp, ki, kd;       // Q15格式参数
    q15_t integral;          // 积分项
    q15_t prev_error;        // 上次的误差
    q15_t output_min, output_max;  // 输出限制
} PID_Fixed;

q15_t pid_compute_fixed(PID_Fixed *pid, q15_t setpoint, q15_t measurement) {
    q15_t error = q15_sub(setpoint, measurement);

    // 积分项
    pid->integral = q15_add(pid->integral, error);
    // 积分限幅...

    // 微分项
    q15_t derivative = q15_sub(error, pid->prev_error);
    pid->prev_error = error;

    // 输出 = kp*error + ki*integral + kd*derivative
    q15_t output = q15_mul(pid->kp, error);
    output = q15_add(output, q15_mul(pid->ki, pid->integral));
    output = q15_add(output, q15_mul(pid->kd, derivative));

    // 输出限幅...
    return output;
}
