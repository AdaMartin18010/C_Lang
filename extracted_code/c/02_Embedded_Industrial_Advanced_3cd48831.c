/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Industrial_Advanced.md
 * Line: 1538
 * Language: c
 * Block ID: 3cd48831
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// FOC: 无刷直流电机(BLDC)和永磁同步电机(PMSM)的高效控制
// 将三相电流转换为d-q坐标系，实现解耦控制
// ============================================================

#include <math.h>

typedef struct {
    float id, iq;      // d轴、q轴电流
    float vd, vq;      // d轴、q轴电压
    float theta;       // 转子电角度
    float speed;       // 转速
} FOC_State;

// Park变换: 三相静止坐标 → 两相旋转坐标
void park_transform(float ia, float ib, float theta, float *id, float *iq) {
    // Clark变换: 三相→两相静止
    float i_alpha = ia;
    float i_beta = (ia + 2.0f * ib) / sqrtf(3.0f);

    // Park变换: 静止→旋转
    float cos_theta = cosf(theta);
    float sin_theta = sinf(theta);

    *id = i_alpha * cos_theta + i_beta * sin_theta;
    *iq = -i_alpha * sin_theta + i_beta * cos_theta;
}

// 反Park变换: 两相旋转坐标 → 三相静止坐标
void inverse_park(float vd, float vq, float theta,
                  float *va, float *vb, float *vc) {
    float cos_theta = cosf(theta);
    float sin_theta = sinf(theta);

    // 反Park: 旋转→静止
    float v_alpha = vd * cos_theta - vq * sin_theta;
    float v_beta = vd * sin_theta + vq * cos_theta;

    // 反Clark: 两相→三相
    *va = v_alpha;
    *vb = (-v_alpha + sqrtf(3.0f) * v_beta) / 2.0f;
    *vc = (-v_alpha - sqrtf(3.0f) * v_beta) / 2.0f;
}

// SVPWM (空间矢量脉宽调制)
void svpwm(float va, float vb, float vc, float vdc,
           float *duty_a, float *duty_b, float *duty_c) {
    // 找到最大和最小电压
    float v_max = fmaxf(va, fmaxf(vb, vc));
    float v_min = fminf(va, fminf(vb, vc));

    // 注入零序分量
    float v_offset = (v_max + v_min) / 2.0f;

    // 归一化到PWM周期
    *duty_a = (va - v_offset) / vdc + 0.5f;
    *duty_b = (vb - v_offset) / vdc + 0.5f;
    *duty_c = (vc - v_offset) / vdc + 0.5f;

    // 限幅
    *duty_a = fmaxf(0.0f, fminf(1.0f, *duty_a));
    *duty_b = fmaxf(0.0f, fminf(1.0f, *duty_b));
    *duty_c = fmaxf(0.0f, fminf(1.0f, *duty_c));
}

// FOC主控制循环
void foc_control_loop(FOC_State *foc, float target_iq, float id_ref) {
    // 1. 读取相电流 (ADC)
    float ia = adc_read_current(0);
    float ib = adc_read_current(1);
    // ic = -(ia + ib)  (假设三相平衡)

    // 2. 读取转子位置 (编码器或观测器)
    foc->theta = encoder_read_angle();

    // 3. Park变换
    park_transform(ia, ib, foc->theta, &foc->id, &foc->iq);

    // 4. 电流环PI控制
    static float id_err_int = 0, iq_err_int = 0;
    const float Kp = 0.5f, Ki = 0.1f;

    float id_err = id_ref - foc->id;
    id_err_int += id_err;
    foc->vd = Kp * id_err + Ki * id_err_int;

    float iq_err = target_iq - foc->iq;
    iq_err_int += iq_err;
    foc->vq = Kp * iq_err + Ki * iq_err_int;

    // 5. 反Park变换
    float va, vb, vc;
    inverse_park(foc->vd, foc->vq, foc->theta, &va, &vb, &vc);

    // 6. SVPWM
    float duty_a, duty_b, duty_c;
    float vdc = adc_read_bus_voltage();
    svpwm(va, vb, vc, vdc, &duty_a, &duty_b, &duty_c);

    // 7. 更新PWM占空比
    pwm_set_duty(PWM_A, duty_a);
    pwm_set_duty(PWM_B, duty_b);
    pwm_set_duty(PWM_C, duty_c);
}
