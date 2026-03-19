/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Eva_Tutorial.md
 * Line: 645
 * Language: c
 * Block ID: 5f109862
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// tutorial8_pid.c
#include <limits.h>

#define PID_MIN -1000
#define PID_MAX  1000

typedef struct {
    float kp, ki, kd;  // 增益
    float integral;
    float prev_error;
    int   first_run;
} PID_Controller;

/*@
  @requires \valid(ctrl);
  @requires ctrl->kp >= 0 && ctrl->ki >= 0 && ctrl->kd >= 0;
  @requires setpoint >= -1000 && setpoint <= 1000;
  @requires measured >= -1000 && measured <= 1000;
  @assigns ctrl->integral, ctrl->prev_error, ctrl->first_run;
  @ensures \result >= PID_MIN && \result <= PID_MAX;
*/
float pid_compute(PID_Controller* ctrl, float setpoint, float measured, float dt) {
    float error = setpoint - measured;

    // 积分项
    ctrl->integral += error * dt;

    // 限制积分 (抗饱和)
    if (ctrl->integral > 100) ctrl->integral = 100;
    if (ctrl->integral < -100) ctrl->integral = -100;

    // 微分项
    float derivative = 0;
    if (!ctrl->first_run) {
        derivative = (error - ctrl->prev_error) / dt;
    } else {
        ctrl->first_run = 0;
    }

    // PID输出
    float output = ctrl->kp * error +
                   ctrl->ki * ctrl->integral +
                   ctrl->kd * derivative;

    // 输出限幅
    if (output > PID_MAX) output = PID_MAX;
    if (output < PID_MIN) output = PID_MIN;

    ctrl->prev_error = error;
    return output;
}
