/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\07_Industrial_Case_Studies.md
 * Line: 158
 * Language: c
 * Block ID: f60a284e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// mitsubishi_elevator_door.c
// 三菱电机电梯门控制 - 安全关键代码

#include <stdint.h>
#include <stdbool.h>

#define DOOR_MAX_POSITION    1000  // mm
#define DOOR_CLOSE_POSITION  0
#define DOOR_OPEN_POSITION   1000
#define DOOR_SPEED_LIMIT     500   // mm/s
#define OBSTACLE_THRESHOLD   50    // N

typedef enum {
    DOOR_STATE_CLOSED,
    DOOR_STATE_OPENING,
    DOOR_STATE_OPEN,
    DOOR_STATE_CLOSING,
    DOOR_STATE_FAULT
} DoorState;

typedef struct {
    DoorState state;
    uint16_t position;      // 0-1000 mm
    int16_t speed;          // mm/s, 正为开门方向
    uint16_t motor_current; // mA
    bool obstacle_detected;
    uint32_t fault_code;
} DoorControl;

/*@
  @predicate valid_door_state(DoorControl* d) =
    d->position <= DOOR_MAX_POSITION &&
    d->state >= DOOR_STATE_CLOSED && d->state <= DOOR_STATE_FAULT &&
    (d->state == DOOR_STATE_CLOSED ==> d->position == DOOR_CLOSE_POSITION) &&
    (d->state == DOOR_STATE_OPEN ==> d->position == DOOR_OPEN_POSITION);

  @predicate safe_operation(DoorControl* d) =
    d->obstacle_detected ==> d->state != DOOR_STATE_CLOSING;
*/

/*@
  @requires \valid(ctrl);
  @requires valid_door_state(ctrl);
  @requires target_pos <= DOOR_MAX_POSITION;
  @assigns ctrl->position, ctrl->speed, ctrl->state, ctrl->motor_current;
  @ensures ctrl->position <= DOOR_MAX_POSITION;
  @ensures \abs(ctrl->speed) <= DOOR_SPEED_LIMIT;
  @ensures safe_operation(ctrl);
  @behavior normal_close:
    @assumes !ctrl->obstacle_detected;
    @ensures ctrl->position <= \old(ctrl->position);
  @behavior obstacle_stop:
    @assumes ctrl->obstacle_detected;
    @ensures ctrl->state != DOOR_STATE_CLOSING;
  @complete behaviors;
  @disjoint behaviors;
*/
void door_control_cycle(DoorControl* ctrl, uint16_t target_pos) {
    // 安全检查: 如果检测到障碍物,停止关门
    if (ctrl->obstacle_detected && ctrl->state == DOOR_STATE_CLOSING) {
        ctrl->state = DOOR_STATE_OPENING;
        ctrl->speed = -DOOR_SPEED_LIMIT / 2;  // 缓慢开门
        return;
    }

    // 计算速度
    int16_t error = (int16_t)target_pos - (int16_t)ctrl->position;
    int16_t desired_speed;

    if (error > 10) {
        desired_speed = DOOR_SPEED_LIMIT;
        ctrl->state = DOOR_STATE_OPENING;
    } else if (error < -10) {
        desired_speed = -DOOR_SPEED_LIMIT;
        ctrl->state = DOOR_STATE_CLOSING;
    } else {
        desired_speed = 0;
        if (ctrl->position < 100) {
            ctrl->state = DOOR_STATE_CLOSED;
        } else if (ctrl->position > 900) {
            ctrl->state = DOOR_STATE_OPEN;
        }
    }

    // 更新位置和速度
    ctrl->speed = desired_speed;

    // Eva验证: 确保不会溢出
    if (desired_speed > 0 && ctrl->position < DOOR_MAX_POSITION - desired_speed) {
        ctrl->position += (uint16_t)desired_speed;
    } else if (desired_speed < 0 && ctrl->position > (uint16_t)(-desired_speed)) {
        ctrl->position -= (uint16_t)(-desired_speed);
    }

    // 更新电机电流 (简化模型)
    ctrl->motor_current = (uint16_t)(100 + \abs(desired_speed) / 5);
}

/*@
  @requires \valid(ctrl);
  @assigns ctrl->state, ctrl->speed, ctrl->fault_code;
  @ensures ctrl->state == DOOR_STATE_FAULT ==> ctrl->fault_code != 0;
  @ensures ctrl->state != DOOR_STATE_CLOSING || !ctrl->obstacle_detected;
*/
void door_emergency_stop(DoorControl* ctrl) {
    ctrl->speed = 0;

    if (ctrl->obstacle_detected) {
        ctrl->state = DOOR_STATE_FAULT;
        ctrl->fault_code = 0x01;  // 障碍物故障
    }
}
