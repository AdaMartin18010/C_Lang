/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\ESP32\01_ESP32_Complete_Guide.md
 * Line: 2701
 * Language: c
 * Block ID: 2316f9a2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * i2c_driver_example.c
 * ESP32 I2C 主从模式完整示例
 * 支持: 标准模式(100KHz), 快速模式(400KHz), 快速模式+(1MHz)
 */

#include "driver/i2c.h"

#define I2C_MASTER_NUM          I2C_NUM_0        /* I2C 端口号 */
#define I2C_MASTER_SDA_IO       GPIO_NUM_21      /* SDA 引脚 */
#define I2C_MASTER_SCL_IO       GPIO_NUM_22      /* SCL 引脚 */
#define I2C_MASTER_FREQ_HZ      400000           /* 400KHz */
#define I2C_MASTER_TX_BUF_DISABLE   0
#define I2C_MASTER_RX_BUF_DISABLE   0
#define I2C_MASTER_TIMEOUT_MS   1000

/* SHT30 温湿度传感器 I2C 地址 */
#define SHT30_ADDR              0x44
#define SHT30_CMD_MEASURE       0x2C06

/**
 * @brief 初始化 I2C 主设备
 */
static esp_err_t i2c_master_init(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    /* 配置 I2C */
    esp_err_t err = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (err != ESP_OK) {
        return err;
    }

    /* 安装驱动 */
    err = i2c_driver_install(I2C_MASTER_NUM, conf.mode,
                             I2C_MASTER_RX_BUF_DISABLE,
                             I2C_MASTER_TX_BUF_DISABLE, 0);

    return err;
}

/**
 * @brief I2C 读取寄存器
 */
static esp_err_t i2c_read_register(uint8_t dev_addr, uint8_t reg_addr,
                                   uint8_t *data, size_t len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    /* 开始信号 */
    i2c_master_start(cmd);

    /* 发送设备地址 + 写 */
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);

    /* 发送寄存器地址 */
    i2c_master_write_byte(cmd, reg_addr, true);

    /* 重新开始 */
    i2c_master_start(cmd);

    /* 发送设备地址 + 读 */
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_READ, true);

    /* 读取数据 */
    if (len > 1) {
        i2c_master_read(cmd, data, len - 1, I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, data + len - 1, I2C_MASTER_NACK);

    /* 停止信号 */
    i2c_master_stop(cmd);

    /* 执行命令 */
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd,
                                          pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));

    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
 * @brief I2C 写入寄存器
 */
static esp_err_t i2c_write_register(uint8_t dev_addr, uint8_t reg_addr,
                                    uint8_t *data, size_t len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    /* 开始信号 */
    i2c_master_start(cmd);

    /* 发送设备地址 + 写 */
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);

    /* 发送寄存器地址 */
    i2c_master_write_byte(cmd, reg_addr, true);

    /* 发送数据 */
    if (len > 0) {
        i2c_master_write(cmd, data, len, true);
    }

    /* 停止信号 */
    i2c_master_stop(cmd);

    /* 执行命令 */
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd,
                                          pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));

    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
 * @brief SHT30 温湿度传感器读取
 */
static esp_err_t sht30_read(float *temperature, float *humidity)
{
    uint8_t cmd[2] = {0x2C, 0x06};  /* 高精度测量命令 */
    uint8_t data[6];                 /* 温湿度 + CRC */

    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();

    /* 发送测量命令 */
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (SHT30_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd_handle, cmd, 2, true);
    i2c_master_stop(cmd_handle);

    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd_handle,
                                          pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd_handle);

    if (ret != ESP_OK) {
        return ret;
    }

    /* 等待测量完成 */
    vTaskDelay(pdMS_TO_TICKS(20));

    /* 读取数据 */
    cmd_handle = i2c_cmd_link_create();
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (SHT30_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd_handle, data, 5, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd_handle, data + 5, I2C_MASTER_NACK);
    i2c_master_stop(cmd_handle);

    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd_handle,
                               pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd_handle);

    if (ret != ESP_OK) {
        return ret;
    }

    /* 解析数据 */
    uint16_t temp_raw = (data[0] << 8) | data[1];
    uint16_t hum_raw = (data[3] << 8) | data[4];

    *temperature = -45 + 175 * ((float)temp_raw / 65535.0f);
    *humidity = 100 * ((float)hum_raw / 65535.0f);

    return ESP_OK;
}

/**
 * @brief I2C 总线扫描
 */
static void i2c_scan(void)
{
    ESP_LOGI(TAG, "开始 I2C 总线扫描...");
    ESP_LOGI(TAG, "     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f");
    ESP_LOGI(TAG, "00:          ");

    for (uint8_t addr = 0x03; addr <= 0x77; addr++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);

        esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd,
                                              pdMS_TO_TICKS(50));
        i2c_cmd_link_delete(cmd);

        if (addr % 16 == 0) {
            printf("\n%02x:", addr);
        }

        if (ret == ESP_OK) {
            printf(" %02x", addr);
        } else {
            printf(" --");
        }
    }
    printf("\n");
}
