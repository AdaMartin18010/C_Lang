/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\ESP32\01_ESP32_Complete_Guide.md
 * Line: 3917
 * Language: c
 * Block ID: 0d2c6d04
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* I2C 调试和修复 */

static void debug_i2c_communication(void)
{
    /* 1. 检查上拉电阻 */
    /* SDA 和 SCL 必须有 4.7K - 10K 上拉电阻 */

    /* 2. 降低 I2C 速度测试 */
    i2c_config_t conf = {
        .master.clk_speed = 50000,  /* 降低到 50KHz */
    };

    /* 3. 增加超时时间 */
    i2c_set_timeout(I2C_MASTER_NUM, 20000);  /* 200ms */

    /* 4. 总线扫描 */
    for (uint8_t addr = 0x08; addr < 0x78; addr++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);

        if (i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(50)) == ESP_OK) {
            ESP_LOGI(TAG, "找到设备: 0x%02X", addr);
        }
        i2c_cmd_link_delete(cmd);
    }
}
