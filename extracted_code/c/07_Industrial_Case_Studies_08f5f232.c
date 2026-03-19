/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\07_Industrial_Case_Studies.md
 * Line: 577
 * Language: c
 * Block ID: 08f5f232
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// aerospace_attitude_control.c
// 卫星姿态控制算法 - DO-178C C级软件

#include <math.h>
#include <stdint.h>

#define MAX_TORQUE      1000    // mNm
#define MAX_ANGLE       1800    // 0.1度
#define CONTROL_PERIOD  100     // ms
#define SAFETY_MARGIN   50      // 安全裕度

typedef struct {
    int16_t roll;     // 滚转角 (0.1度)
    int16_t pitch;    // 俯仰角 (0.1度)
    int16_t yaw;      // 偏航角 (0.1度)
} Attitude;

typedef struct {
    int16_t torque_x; // X轴力矩
    int16_t torque_y; // Y轴力矩
    int16_t torque_z; // Z轴力矩
} ControlOutput;

/*@
  @predicate valid_attitude(Attitude* a) =
    -MAX_ANGLE <= a->roll <= MAX_ANGLE &&
    -MAX_ANGLE <= a->pitch <= MAX_ANGLE &&
    -MAX_ANGLE <= a->yaw <= MAX_ANGLE;

  @predicate safe_control_output(ControlOutput* c) =
    -MAX_TORQUE <= c->torque_x <= MAX_TORQUE &&
    -MAX_TORQUE <= c->torque_y <= MAX_TORQUE &&
    -MAX_TORQUE <= c->torque_z <= MAX_TORQUE;
*/

/*@
  @requires \valid(current) && \valid(target);
  @requires valid_attitude(current);
  @requires valid_attitude(target);
  @requires \valid(output);
  @assigns output->torque_x, output->torque_y, output->torque_z;

  // 安全关键: 输出力矩必须在安全范围内
  @ensures safe_control_output(output);

  // 稳定性: 当误差为零时,输出为零
  @ensures (current->roll == target->roll &&
           current->pitch == target->pitch &&
           current->yaw == target->yaw) ==>
          (output->torque_x == 0 &&
           output->torque_y == 0 &&
           output->torque_z == 0);

  @behavior normal_operation:
    @assumes \abs(current->roll - target->roll) <= MAX_ANGLE;
    @ensures safe_control_output(output);

  @behavior fault_condition:
    @assumes \abs(current->roll - target->roll) > MAX_ANGLE;
    @ensures output->torque_x == 0;  // 安全模式
*/
void attitude_control(const Attitude* current,
                      const Attitude* target,
                      ControlOutput* output) {
    // 计算姿态误差
    int32_t error_roll = (int32_t)target->roll - (int32_t)current->roll;
    int32_t error_pitch = (int32_t)target->pitch - (int32_t)current->pitch;
    int32_t error_yaw = (int32_t)target->yaw - (int32_t)current->yaw;

    // 安全检查: 误差是否过大 (传感器故障检测)
    if (error_roll > MAX_ANGLE || error_roll < -MAX_ANGLE) {
        // 传感器故障,进入安全模式
        output->torque_x = 0;
        output->torque_y = 0;
        output->torque_z = 0;
        return;
    }

    // PID控制 (简化版)
    int32_t torque_x = (error_roll * 10) / CONTROL_PERIOD;  // P控制
    int32_t torque_y = (error_pitch * 10) / CONTROL_PERIOD;
    int32_t torque_z = (error_yaw * 10) / CONTROL_PERIOD;

    // 输出限幅 (安全关键)
    if (torque_x > MAX_TORQUE - SAFETY_MARGIN) {
        torque_x = MAX_TORQUE - SAFETY_MARGIN;
    } else if (torque_x < -(MAX_TORQUE - SAFETY_MARGIN)) {
        torque_x = -(MAX_TORQUE - SAFETY_MARGIN);
    }

    if (torque_y > MAX_TORQUE - SAFETY_MARGIN) {
        torque_y = MAX_TORQUE - SAFETY_MARGIN;
    } else if (torque_y < -(MAX_TORQUE - SAFETY_MARGIN)) {
        torque_y = -(MAX_TORQUE - SAFETY_MARGIN);
    }

    if (torque_z > MAX_TORQUE - SAFETY_MARGIN) {
        torque_z = MAX_TORQUE - SAFETY_MARGIN;
    } else if (torque_z < -(MAX_TORQUE - SAFETY_MARGIN)) {
        torque_z = -(MAX_TORQUE - SAFETY_MARGIN);
    }

    output->torque_x = (int16_t)torque_x;
    output->torque_y = (int16_t)torque_y;
    output->torque_z = (int16_t)torque_z;
}
