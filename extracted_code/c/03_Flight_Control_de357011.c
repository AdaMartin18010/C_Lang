/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Avionics_Systems\03_Flight_Control.md
 * Line: 139
 * Language: c
 * Block ID: de357011
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 符合DO-178C DAL A (灾难级)要求的代码

// 需求: FC-001 俯仰角保护
// 验证: 当俯仰角超过25度时，系统自动推杆

#define PITCH_LIMIT_DEG 25.0f
#define RECOVERY_RATE_DEG_S 5.0f

float pitch_protection(float pilot_input, float pitch_angle) {
    // 前置条件检查
    if (!isfinite(pitch_angle)) {
        // 故障处理
        return 0.0f;
    }

    float output = pilot_input;

    // 上仰保护
    if (pitch_angle > PITCH_LIMIT_DEG && pilot_input > 0) {
        output = -RECOVERY_RATE_DEG_S;  // 自动推杆
    }

    // 下俯保护
    if (pitch_angle < -PITCH_LIMIT_DEG && pilot_input < 0) {
        output = RECOVERY_RATE_DEG_S;   // 自动拉杆
    }

    return output;
}
