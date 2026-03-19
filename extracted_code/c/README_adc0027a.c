/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Automotive_ECU\README.md
 * Line: 630
 * Language: c
 * Block ID: adc0027a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file engine_control.c
 * @brief 发动机控制ECU - 点火正时计算示例
 */

#include "engine_control.h"

/* 曲轴位置 */
typedef struct {
    uint16_t rpm;              /* 发动机转速 */
    uint16_t crank_angle;      /* 曲轴角度 (0-720度, 四冲程) */
    uint8_t cylinder_phase;    /* 气缸相位 */
} EnginePosition_t;

/* 点火参数 */
typedef struct {
    uint16_t ignition_advance; /* 点火提前角 (0.1度) */
    uint16_t dwell_time_us;    /* 充电时间 */
    uint8_t coil_number;       /* 点火线圈号 */
} IgnitionTiming_t;

/* 点火MAP (简化示例: 转速x负荷) */
static const uint16_t ignition_map[16][16] = {
    /* 负荷: 低 → 高 */
    /* 转速: 低 → 高 */
    {50, 50, 45, 40, 35, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},  /* 500 RPM */
    {50, 50, 45, 40, 35, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32},
    /* ... 更多行 ... */
    {35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 30, 30, 28, 28, 28, 28},  /* 6000 RPM */
};

/**
 * @brief 计算点火正时
 */
void CalculateIgnitionTiming(EnginePosition_t *pos,
                              EngineLoad_t *load,
                              IgnitionTiming_t *timing)
{
    uint8_t rpm_idx, load_idx;
    uint16_t base_advance;
    int16_t corrections = 0;

    /* MAP索引计算 */
    rpm_idx = (pos->rpm < 500) ? 0 : (pos->rpm - 500) / 367;
    if (rpm_idx > 15) rpm_idx = 15;

    load_idx = (uint8_t)(load->percent / 6.25f);
    if (load_idx > 15) load_idx = 15;

    /* 基础点火提前角 */
    base_advance = ignition_map[rpm_idx][load_idx];

    /* 冷却液温度修正 */
    if (load->coolant_temp < 70) {
        corrections += 5;  /* 冷启动加浓 */
    }

    /* 爆震修正 */
    if (load->knock_detected) {
        corrections -= 3;
    }

    /* 最终点火提前角 */
    timing->ignition_advance = base_advance + corrections;

    /* 计算充电时间 (基于线圈特性) */
    timing->dwell_time_us = 3000;  /* 3ms */

    /* 计算点火线圈 */
    timing->coil_number = (pos->cylinder_phase % 4);
}

/**
 * @brief 点火控制ISR (曲轴每度触发)
 */
void CRANK_IRQHandler(void)
{
    static EnginePosition_t pos = {0};
    static IgnitionTiming_t timing;
    EngineLoad_t load;

    /* 更新曲轴位置 */
    pos.crank_angle++;
    if (pos.crank_angle >= 720) {
        pos.crank_angle = 0;
        pos.cylinder_phase = 0;
    }

    /* 每90度计算一次点火 (四冲程8次点火) */
    if (pos.crank_angle % 90 == 0) {
        /* 读取传感器 */
        load.percent = ADC_Read(ADC_CHANNEL_MAP);  /* 进气歧管压力 */
        load.coolant_temp = ADC_Read(ADC_CHANNEL_CTS);
        load.knock_detected = GPIO_Read(KNOCK_PIN);

        /* 计算点火正时 */
        CalculateIgnitionTiming(&pos, &load, &timing);

        /* 设置点火定时器 */
        uint16_t ignition_angle = 360 - timing.ignition_advance;
        if (pos.crank_angle >= ignition_angle - 30) {
            /* 开始充电 (dwell) */
            SetCoilCharge(timing.coil_number, true);
        }
        if (pos.crank_angle >= ignition_angle) {
            /* 点火 */
            SetCoilCharge(timing.coil_number, false);
            pos.cylinder_phase++;
        }
    }
}
