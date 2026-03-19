/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\ESP32\01_ESP32_Complete_Guide.md
 * Line: 2911
 * Language: c
 * Block ID: aa61ec3b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * spi_driver_example.c
 * ESP32 SPI 主设备完整示例
 * 支持: SPI1(Flash), SPI2(HSPI), SPI3(VSPI)
 */

#include "driver/spi_master.h"
#include "driver/gpio.h"

#define SPI_HOST                SPI2_HOST       /* SPI2/HSPI */
#define PIN_NUM_MISO            GPIO_NUM_19
#define PIN_NUM_MOSI            GPIO_NUM_23
#define PIN_NUM_CLK             GPIO_NUM_18
#define PIN_NUM_CS              GPIO_NUM_5
#define SPI_DMA_CHAN            SPI_DMA_CH_AUTO

/* ILI9341 LCD 命令 */
#define ILI9341_SLPOUT          0x11
#define ILI9341_DISPON          0x29
#define ILI9341_CASET           0x2A
#define ILI9341_PASET           0x2B
#define ILI9341_RAMWR           0x2C

static spi_device_handle_t spi = NULL;

/**
 * @brief 初始化 SPI 主设备
 */
static esp_err_t spi_master_init(void)
{
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4094,  /* 最大传输大小 */
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 26 * 1000 * 1000,  /* 26 MHz */
        .mode = 0,                           /* SPI 模式 0 (CPOL=0, CPHA=0) */
        .spics_io_num = PIN_NUM_CS,          /* CS 引脚 */
        .queue_size = 7,                     /* 事务队列大小 */
        .pre_cb = NULL,                      /* 传输前回调 */
    };

    /* 初始化 SPI 总线 */
    esp_err_t ret = spi_bus_initialize(SPI_HOST, &buscfg, SPI_DMA_CHAN);
    if (ret != ESP_OK) {
        return ret;
    }

    /* 添加设备 */
    ret = spi_bus_add_device(SPI_HOST, &devcfg, &spi);

    return ret;
}

/**
 * @brief SPI 传输数据
 */
static esp_err_t spi_transfer(const uint8_t *tx_data, uint8_t *rx_data, size_t len)
{
    spi_transaction_t t;

    memset(&t, 0, sizeof(t));
    t.length = len * 8;  /* 位长度 */
    t.tx_buffer = tx_data;
    t.rx_buffer = rx_data;
    t.user = (void *)1;  /* 用户数据 */

    return spi_device_polling_transmit(spi, &t);
}

/**
 * @brief 向 ILI9341 发送命令
 */
static void lcd_cmd(uint8_t cmd)
{
    gpio_set_level(PIN_NUM_DC, 0);  /* DC = 0 (命令) */

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &cmd;

    spi_device_polling_transmit(spi, &t);
}

/**
 * @brief 向 ILI9341 发送数据
 */
static void lcd_data(const uint8_t *data, int len)
{
    gpio_set_level(PIN_NUM_DC, 1);  /* DC = 1 (数据) */

    if (len == 0) return;

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = len * 8;
    t.tx_buffer = data;

    spi_device_polling_transmit(spi, &t);
}

/**
 * @brief SPI 队列传输 (异步)
 */
static void spi_queue_example(void)
{
    /* 准备多个事务 */
    spi_transaction_t trans[3];
    uint8_t tx_buf1[10] = {0x01, 0x02, 0x03};
    uint8_t tx_buf2[10] = {0x04, 0x05, 0x06};
    uint8_t rx_buf[10];

    /* 事务 1: 仅发送 */
    memset(&trans[0], 0, sizeof(spi_transaction_t));
    trans[0].length = 3 * 8;
    trans[0].tx_buffer = tx_buf1;

    /* 事务 2: 仅接收 */
    memset(&trans[1], 0, sizeof(spi_transaction_t));
    trans[1].length = 3 * 8;
    trans[1].rx_buffer = rx_buf;

    /* 事务 3: 同时收发 */
    memset(&trans[2], 0, sizeof(spi_transaction_t));
    trans[2].length = 3 * 8;
    trans[2].tx_buffer = tx_buf2;
    trans[2].rx_buffer = rx_buf;

    /* 将事务加入队列 */
    for (int i = 0; i < 3; i++) {
        spi_device_queue_trans(spi, &trans[i], portMAX_DELAY);
    }

    /* 等待所有事务完成 */
    spi_transaction_t *rtrans;
    for (int i = 0; i < 3; i++) {
        spi_device_get_trans_result(spi, &rtrans, portMAX_DELAY);
    }
}
