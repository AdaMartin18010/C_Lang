/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\01_ABS_System.md
 * Line: 3268
 * Language: c
 * Block ID: 08f5ddf0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*============================================================================
 * TRAP 10: 标定参数错误导致的性能问题
 *============================================================================*/

/* ❌ 错误：固定标定参数不考虑车辆差异 */
/* 同一套参数用于所有车型 */
static const float slip_threshold = 0.15f;  /* 固定值 */

/* ✅ 正确：可标定参数 + 车型适配 */
/* 使用XCP/CCP进行在线标定 */
typedef struct {
    float slip_threshold_enter;     /* 可标定 */
    float slip_threshold_exit;      /* 可标定 */
    float decel_threshold;          /* 可标定 */
    uint16_t time_hold;             /* 可标定 */
} ABS_CalibrationParams;

/* 不同车型的标定值 */
ABS_CalibrationParams abs_cal_compact = {
    .slip_threshold_enter = 0.15f,
    .slip_threshold_exit = 0.08f,
    .decel_threshold = -8.0f,
    .time_hold = 20
};

ABS_CalibrationParams abs_cal_truck = {
    .slip_threshold_enter = 0.12f,  /* 货车需要更保守 */
    .slip_threshold_exit = 0.06f,
    .decel_threshold = -6.0f,        /* 货车减速度阈值更低 */
    .time_hold = 30                  /* 保压时间更长 */
};

/* 标定验证 */
void validate_calibration(void) {
    /* 检查参数范围 */
    assert(cal_params.slip_threshold_enter > 0.05f &&
           cal_params.slip_threshold_enter < 0.40f);
    assert(cal_params.slip_threshold_exit < cal_params.slip_threshold_enter);
    assert(cal_params.time_hold >= 10 && cal_params.time_hold <= 100);

    /* CRC校验 */
    if (CRC_Calculate(&cal_params) != stored_crc) {
        /* 使用默认参数并报告故障 */
        use_default_calibration();
        report_fault(FAULT_CALIBRATION_ERROR);
    }
}
