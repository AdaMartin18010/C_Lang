/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\IEC_61508\01_SIL_Implementation.md
 * Line: 83
 * Language: c
 * Block ID: 99150fa2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * SIL 3安全功能示例：紧急停车系统
 */

/* 安全需求规范 */
#define SAFETY_REQ_001 "系统在检测到超压时应在100ms内关闭阀门"
#define SAFETY_REQ_002 "系统失效应进入安全状态（阀门关闭）"
#define SAFETY_REQ_003 "诊断覆盖率应达到99%"

/* 安全功能 */
typedef struct {
    bool pressure_ok;
    bool valve_closed;
    uint32_t response_time_ms;
    uint32_t diagnostic_coverage;  /* 0-10000 (0.01%) */
} SafetyFunction;

SIL_Level execute_emergency_shutdown(SafetyFunction *sf)
{
    uint32_t start_time = get_time_ms();

    /* 读取传感器 */
    float pressure = read_pressure_sensor();

    /* 诊断检查 */
    if (!verify_sensor_integrity()) {
        sf->pressure_ok = false;
        trigger_alarm(SENSOR_FAULT);
    } else {
        sf->pressure_ok = (pressure < MAX_PRESSURE);
    }

    /* 执行安全动作 */
    if (!sf->pressure_ok) {
        close_valve();
        sf->valve_closed = true;
    }

    sf->response_time_ms = get_time_ms() - start_time;

    /* 验证响应时间 */
    if (sf->response_time_ms > 100) {
        log_safety_event(RESPONSE_TIME_EXCEEDED);
    }

    return SIL_3;
}
