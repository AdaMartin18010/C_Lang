/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\01_ABS_System.md
 * Line: 2987
 * Language: c
 * Block ID: a552f957
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*============================================================================
 * TRAP 04: 对开路面（Split-μ）处理不当
 *============================================================================*/

/* ❌ 错误做法：独立控制导致横摆力矩失控 */
void bad_split_mu_control(void) {
    /* 左右轮独立ABS控制 */
    for (i = 0; i < 2; i++) {  /* 前轮 */
        if (slip[i] > threshold) {
            decrease_pressure(i);  /* 低附侧频繁减压 */
        }
    }
    /* 结果：左右制动力不均，产生横摆力矩 */
}

/* ✅ 正确做法：选择性低压控制 + 横摆补偿 */
void good_split_mu_control(void) {
    float slip_fl = get_slip(FL);
    float slip_fr = get_slip(FR);
    float pressure_fl = get_pressure(FL);
    float pressure_fr = get_pressure(FR);

    /* 检测对开路面 */
    if (fabs(slip_fl - slip_fr) > SPLIT_MU_THRESHOLD) {
        /* 对开路面模式激活 */

        if (slip_fl > slip_fr) {
            /* 左侧附着系数低 */
            /* 限制低附侧压力，使其与高附侧匹配 */
            float limit_pressure = pressure_fr * 0.7f;
            if (pressure_fl > limit_pressure) {
                set_pressure(FL, limit_pressure);
            }
        } else {
            /* 右侧附着系数低 */
            float limit_pressure = pressure_fl * 0.7f;
            if (pressure_fr > limit_pressure) {
                set_pressure(FR, limit_pressure);
            }
        }

        /* 如果仍有横摆趋势，ESP介入 */
        if (fabs(yaw_rate) > YAW_THRESHOLD) {
            request_esp_intervention();
        }
    }
}
