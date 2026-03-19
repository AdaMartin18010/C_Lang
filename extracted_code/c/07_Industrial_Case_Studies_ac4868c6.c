/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\07_Industrial_Case_Studies.md
 * Line: 456
 * Language: c
 * Block ID: ac4868c6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// autosar_brake_system.c
#include <stdint.h>
#include <stdbool.h>

#define MAX_BRAKE_PRESSURE  10000  // 0.1 bar units
#define MAX_WHEEL_SPEED     30000  // 0.01 km/h
#define ABS_THRESHOLD       500    // 速度差阈值

// 制动系统状态
typedef struct {
    uint16_t brake_pressure;     // 制动压力
    uint16_t wheel_speed[4];     // 四轮速度
    bool abs_active;             // ABS激活状态
    uint8_t fault_code;          // 故障码
} BrakeSystemState;

/*@
  @predicate valid_brake_state(BrakeSystemState* s) =
    s->brake_pressure <= MAX_BRAKE_PRESSURE &&
    \forall integer i; 0 <= i < 4 ==> s->wheel_speed[i] <= MAX_WHEEL_SPEED;

  @predicate abs_conditions_met(BrakeSystemState* s) =
    // 至少一个轮速显著低于其他轮
    \exists integer i; 0 <= i < 4 ==>
      (\forall integer j; 0 <= j < 4 && j != i ==>
        s->wheel_speed[j] - s->wheel_speed[i] > ABS_THRESHOLD);
*/

/*@
  @requires \valid(state);
  @requires valid_brake_state(state);
  @assigns state->abs_active, state->fault_code;

  @ensures state->abs_active == abs_conditions_met(\old(state));
  @ensures !state->abs_active ==> state->fault_code == 0;

  @behavior abs_activate:
    @assumes abs_conditions_met(state);
    @ensures state->abs_active == true;
    @ensures state->fault_code == 0;

  @behavior abs_deactivate:
    @assumes !abs_conditions_met(state);
    @ensures state->abs_active == false;

  @complete behaviors;
  @disjoint behaviors;
*/
void brake_control_logic(BrakeSystemState* state) {
    // 计算最大和最小轮速
    uint16_t max_speed = 0;
    uint16_t min_speed = MAX_WHEEL_SPEED;

    /*@
      @loop invariant 0 <= i <= 4;
      @loop invariant max_speed ==
        (i == 0 ? 0 : \max(0, (\max(\old(state->wheel_speed[0]), ...))));
      @loop assigns i, max_speed, min_speed;
      @loop variant 4 - i;
    */
    for (int i = 0; i < 4; i++) {
        if (state->wheel_speed[i] > max_speed) {
            max_speed = state->wheel_speed[i];
        }
        if (state->wheel_speed[i] < min_speed) {
            min_speed = state->wheel_speed[i];
        }
    }

    // ABS决策
    if (max_speed - min_speed > ABS_THRESHOLD && state->brake_pressure > 1000) {
        state->abs_active = true;
        state->fault_code = 0;
    } else {
        state->abs_active = false;
    }
}
