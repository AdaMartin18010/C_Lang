/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\ISO_26262\01_ASIL_Decomposition.md
 * Line: 28
 * Language: c
 * Block ID: 186abe1b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 示例: 制动系统冗余
 * 两个独立的ASIL B通道共同实现ASIL D功能
 */

/* 通道A - ASIL B */
typedef struct {
    int32_t pedal_position;
    uint32_t checksum;
    uint8_t status;
} BrakeChannelA;

/* 通道B - ASIL B (独立设计) */
typedef struct {
    float pedal_force;
    uint16_t crc16;
    uint8_t state;
} BrakeChannelB;

/* 比较器 - ASIL D */
typedef struct {
    BrakeChannelA ch_a;
    BrakeChannelB ch_b;
    bool valid;
} BrakeSystem;

int32_t get_brake_force(const BrakeSystem *sys)
{
    /* 独立读取两个通道 */
    int32_t force_a = calculate_force_a(&sys->ch_a);
    int32_t force_b = calculate_force_b(&sys->ch_b);

    /* 交叉比较 */
    if (abs(force_a - force_b) > THRESHOLD) {
        /* 不一致 - 进入安全状态 */
        return enter_safe_state();
    }

    /* 输出取较保守值 */
    return (force_a < force_b) ? force_a : force_b;
}
