/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\01_Memory_Constrained_Design.md
 * Line: 3278
 * Language: c
 * Block ID: d736beb6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 电池管理 - 延长电池寿命
 * ============================================================================ */

/* 电池类型 */
typedef enum {
    BATTERY_LITHIUM_3V,      /* CR2032等 */
    BATTERY_ALKALINE_1_5V,   /* AAA/AA */
    BATTERY_LIPO_3_7V        /* 锂电池 */
} battery_type_t;

/* 电池状态 */
typedef struct {
    battery_type_t type;
    uint16_t voltage_mv;     /* 当前电压 */
    uint8_t percentage;      /* 剩余电量 % */
    uint16_t cycles;         /* 充放电循环 */
    bool is_charging;        /* 是否充电中 */
} battery_status_t;

static battery_status_t battery;

/* ADC读取电池电压 */
uint16_t read_battery_voltage(void)
{
    /* 启动ADC转换 */
    ADC1->SQR3 = BATTERY_ADC_CHANNEL;
    ADC1->CR2 |= ADC_CR2_SWSTART;

    /* 等待完成 */
    while (!(ADC1->SR & ADC_SR_EOC));

    uint16_t adc_value = ADC1->DR;

    /* 计算电压 (根据分压电阻校准) */
    uint32_t voltage = (adc_value * 3300 * 2) / 4096;

    return (uint16_t)voltage;
}

/* 计算电池百分比 */
uint8_t calculate_battery_percentage(uint16_t voltage_mv, battery_type_t type)
{
    switch (type) {
        case BATTERY_LITHIUM_3V:
            /* CR2032: 3.0V满电, 2.0V空 */
            if (voltage_mv >= 3000) return 100;
            if (voltage_mv <= 2000) return 0;
            return (uint8_t)((voltage_mv - 2000) / 10);

        case BATTERY_LIPO_3_7V:
            /* LiPo: 4.2V满电, 3.0V空 (简化线性) */
            if (voltage_mv >= 4200) return 100;
            if (voltage_mv <= 3000) return 0;
            return (uint8_t)((voltage_mv - 3000) / 12);

        default:
            return 0;
    }
}

/* 电池监控任务 */
void battery_monitor_task(void)
{
    static uint32_t last_check = 0;
    uint32_t current_time = get_tick_ms();

    /* 每60秒检查一次 */
    if (current_time - last_check < 60000) return;
    last_check = current_time;

    battery.voltage_mv = read_battery_voltage();
    battery.percentage = calculate_battery_percentage(battery.voltage_mv, battery.type);

    /* 低电量警告 */
    if (battery.percentage < 20) {
        set_low_power_mode();
        reduce_transmission_rate();
    }

    /* 极低电量 - 进入休眠 */
    if (battery.percentage < 5) {
        enter_emergency_sleep();
    }
}

/* 低功耗模式设置 */
void set_low_power_mode(void)
{
    /* 降低采样率 */
    set_sample_interval(60000);  /* 1分钟一次 */

    /* 降低发送频率 */
    set_transmit_interval(300000);  /* 5分钟一次 */

    /* 降低CPU频率 */
    set_cpu_speed(CPU_SPEED_1MHZ);

    /* 禁用LED等非必要功能 */
    disable_status_led();
}
