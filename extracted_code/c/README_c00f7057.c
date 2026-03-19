/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\README.md
 * Line: 1382
 * Language: c
 * Block ID: c00f7057
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* safety/error_handler.c */
#include "safety/error_handler.h"
#include "platform/nvm.h"
#include "network/can_driver.h"

#define MAX_ERROR_RECORDS 32
#define ERROR_LOG_ADDRESS   0x080F0000  /* Flash 日志区域 */

static error_record_t g_error_log[MAX_ERROR_RECORDS];
static uint32_t g_error_count = 0;
static uint32_t g_critical_error_count = 0;
static recovery_action_t g_recovery_actions[256] = {NULL};

void error_report(error_severity_t severity,
                  error_module_t module,
                  uint8_t code,
                  uint32_t parameter) {
    uint32_t pc, lr;

    /* 获取调用上下文 */
    __asm volatile ("mov %0, pc" : "=r" (pc));
    __asm volatile ("mov %0, lr" : "=r" (lr));

    error_report_with_context(severity, module, code, parameter, pc, lr);
}

void error_report_with_context(error_severity_t severity,
                               error_module_t module,
                               uint8_t code,
                               uint32_t parameter,
                               uint32_t pc,
                               uint32_t lr) {
    error_record_t record;

    record.timestamp_ms = get_system_tick();
    record.error_code = MAKE_ERROR_CODE(module, severity, code);
    record.parameter = parameter;
    record.pc = pc;
    record.lr = lr;

    /* 记录到内存 */
    if (g_error_count < MAX_ERROR_RECORDS) {
        g_error_log[g_error_count++] = record;
    }

    /* 写入非易失性存储 */
    nvm_write(ERROR_LOG_ADDRESS + (g_error_count * sizeof(error_record_t)),
              &record, sizeof(record));

    /* 通过 CAN 发送诊断信息 */
    can_frame_t diag_frame;
    diag_frame.id = 0x7FF;  /* 诊断帧 ID */
    diag_frame.dlc = 8;
    pack_u16(&diag_frame.data[0], record.error_code);
    pack_u32(&diag_frame.data[2], record.parameter);
    pack_u16(&diag_frame.data[6], (uint16_t)(record.timestamp_ms / 1000));
    can_send(can_get_controller(0), &diag_frame, 10);

    /* 根据严重度处理 */
    switch (severity) {
        case SEVERITY_INFO:
        case SEVERITY_WARNING:
            /* 继续运行，记录日志 */
            break;

        case SEVERITY_ERROR:
            /* 尝试恢复 */
            if (g_recovery_actions[module]) {
                g_recovery_actions[module]();
            }
            break;

        case SEVERITY_CRITICAL:
            g_critical_error_count++;
            if (g_critical_error_count >= 3) {
                /* 过多严重错误，进入安全状态 */
                enter_safe_state();
            } else {
                /* 单次严重错误，尝试恢复或降级 */
                if (g_recovery_actions[module]) {
                    g_recovery_actions[module]();
                } else {
                    enter_safe_state();
                }
            }
            break;
    }
}

void enter_safe_state(void) {
    /* 禁用中断 */
    __disable_irq();

    /* 关闭 ABS 控制 */
    abs_controller_enable(NULL, false);

    /* 打开所有电磁阀 (释放压力) */
    pwm_set_duty(PWM_INLET_VALVE(0), 255);
    pwm_set_duty(PWM_INLET_VALVE(1), 255);
    pwm_set_duty(PWM_INLET_VALVE(2), 255);
    pwm_set_duty(PWM_INLET_VALVE(3), 255);

    pwm_set_duty(PWM_OUTLET_VALVE(0), 255);
    pwm_set_duty(PWM_OUTLET_VALVE(1), 255);
    pwm_set_duty(PWM_OUTLET_VALVE(2), 255);
    pwm_set_duty(PWM_OUTLET_VALVE(3), 255);

    /* 关闭泵电机 */
    pwm_set_duty(PWM_PUMP_MOTOR(0), 0);

    /* 设置故障灯 */
    gpio_set(ABS_WARNING_LED, true);

    /* 发送故障状态到 CAN */
    can_frame_t fault_frame;
    fault_frame.id = CAN_ID_ABS_STATUS;
    fault_frame.dlc = 1;
    fault_frame.data[0] = 0xFF;  /* 故障状态 */
    can_send(can_get_controller(0), &fault_frame, 10);

    /* 记录故障 */
    error_record_t fault_record;
    fault_record.timestamp_ms = get_system_tick();
    fault_record.error_code = MAKE_ERROR_CODE(ERR_MODULE_ECU, SEVERITY_CRITICAL, 0xFF);
    fault_record.parameter = g_critical_error_count;
    nvm_write(ERROR_LOG_ADDRESS, &fault_record, sizeof(fault_record));

    /* 系统复位或等待看门狗 */
    #ifdef SAFE_STATE_RESET
        system_reset();
    #else
        while (1) {
            /* 闪烁故障灯 */
            gpio_toggle(ABS_WARNING_LED);
            delay_ms(500);
        }
    #endif
}

void system_reset(void) {
    /* 触发系统复位 */
    *(volatile uint32_t*)0xE000ED0C = 0x05FA0004;  /* AIRCR: System Reset */
}
