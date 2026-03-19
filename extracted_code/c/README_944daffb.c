/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\README.md
 * Line: 343
 * Language: c
 * Block ID: 944daffb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 航天器热控制
typedef struct {
    float temp_cpu;
    float temp_fpga;
    float temp_battery;
    float temp_radiator;
} ThermalState;

typedef enum {
    MODE_NORMAL,
    MODE_POWER_SAVE,
    MODE_EMERGENCY
} ThermalMode;

void thermal_management_task(void) {
    ThermalMode mode = MODE_NORMAL;
    ThermalState state;

    while (1) {
        read_thermal_sensors(&state);

        switch (mode) {
            case MODE_NORMAL:
                if (state.temp_cpu > 60.0f ||
                    state.temp_fpga > 70.0f) {
                    mode = MODE_POWER_SAVE;
                    reduce_clock_frequency(50);
                }
                break;

            case MODE_POWER_SAVE:
                if (state.temp_cpu > 75.0f) {
                    mode = MODE_EMERGENCY;
                    shutdown_non_critical_systems();
                } else if (state.temp_cpu < 50.0f) {
                    mode = MODE_NORMAL;
                    restore_clock_frequency();
                }
                break;

            case MODE_EMERGENCY:
                if (state.temp_cpu > 85.0f) {
                    trigger_safe_shutdown();
                } else if (state.temp_cpu < 65.0f) {
                    mode = MODE_POWER_SAVE;
                    restart_critical_systems();
                }
                break;
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
