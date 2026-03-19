/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\01_ABS_System.md
 * Line: 2925
 * Language: c
 * Block ID: e612ae8a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*============================================================================
 * TRAP 03: 低附路面的误判和过调节
 *============================================================================*/

/* ❌ 错误做法：单一阈值控制 */
void bad_low_mu_detection(float decel) {
    if (decel < -8.0f) {
        /* 在高附路面正常，但在冰面上可能过度反应 */
        enter_abs();
    }
}

/* ✅ 正确做法：路面识别 + 自适应阈值 */
typedef enum {
    ROAD_HIGH_MU,       /* 干燥沥青 */
    ROAD_MEDIUM_MU,     /* 潮湿路面 */
    ROAD_LOW_MU,        /* 积雪路面 */
    ROAD_ICE            /* 冰面 */
} RoadConditionType;

void good_low_mu_handling(void) {
    static RoadConditionType road_condition = ROAD_HIGH_MU;
    static float mu_estimate = 0.8f;

    /* 1. 路面附着系数估计 */
    mu_estimate = estimate_road_mu(accel, slip);

    /* 2. 路面分类 */
    if (mu_estimate > 0.6f) {
        road_condition = ROAD_HIGH_MU;
    } else if (mu_estimate > 0.4f) {
        road_condition = ROAD_MEDIUM_MU;
    } else if (mu_estimate > 0.2f) {
        road_condition = ROAD_LOW_MU;
    } else {
        road_condition = ROAD_ICE;
    }

    /* 3. 自适应控制参数 */
    switch (road_condition) {
        case ROAD_HIGH_MU:
            slip_target = 0.15f;
            decel_threshold = -8.0f;
            pressure_inc_rate = FAST;
            break;

        case ROAD_LOW_MU:
        case ROAD_ICE:
            slip_target = 0.08f;        /* 更低的目标滑移率 */
            decel_threshold = -3.0f;     /* 更早触发 */
            pressure_inc_rate = SLOW;    /* 更慢的增压 */
            break;

        default:
            break;
    }
}
