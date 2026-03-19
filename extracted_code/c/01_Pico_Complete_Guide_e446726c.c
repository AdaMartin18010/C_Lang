/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 1787
 * Language: c
 * Block ID: e446726c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file spi_example.c
 * @brief SPI 通信示例 - W25Q128 闪存 + ILI9341 TFT 显示屏
 */

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <stdio.h>
#include <string.h>

// SPI0 配置 (用于 Flash)
#define SPI_PORT_FLASH  spi0
#define FLASH_MISO_PIN  16
#define FLASH_MOSI_PIN  19
#define FLASH_SCK_PIN   18
#define FLASH_CS_PIN    17

// SPI1 配置 (用于 TFT 显示屏)
#define SPI_PORT_TFT    spi1
#define TFT_MISO_PIN    12
#define TFT_MOSI_PIN    15
#define TFT_SCK_PIN     14
#define TFT_CS_PIN      13
#define TFT_DC_PIN      11
#define TFT_RST_PIN     10

// W25Q128 命令
#define W25X_WRITE_ENABLE       0x06
#define W25X_WRITE_DISABLE      0x04
#define W25X_READ_STATUS_REG    0x05
#define W25X_READ_DATA          0x03
#define W25X_PAGE_PROGRAM       0x02
#define W25X_SECTOR_ERASE       0x20
#define W25X_CHIP_ERASE         0xC7
#define W25X_JEDEC_ID           0x9F

// ILI9341 命令
#define ILI9341_SWRESET         0x01
#define ILI9341_SLPOUT          0x11
#define ILI9341_DISPON          0x29
#define ILI9341_CASET           0x2A
#define ILI9341_PASET           0x2B
#define ILI9341_RAMWR           0x2C

// ==================== Flash 操作 ====================

void flash_cs_select(void) {
    gpio_put(FLASH_CS_PIN, 0);
    sleep_us(1);
}

void flash_cs_deselect(void) {
    sleep_us(1);
    gpio_put(FLASH_CS_PIN, 1);
}

void flash_init(void) {
    // 初始化 SPI
    spi_init(SPI_PORT_FLASH, 1000000);  // 1MHz

    // 配置 GPIO
    gpio_set_function(FLASH_MISO_PIN, GPIO_FUNC_SPI);
    gpio_set_function(FLASH_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(FLASH_SCK_PIN, GPIO_FUNC_SPI);

    // 配置 CS 引脚
    gpio_init(FLASH_CS_PIN);
    gpio_set_dir(FLASH_CS_PIN, GPIO_OUT);
    flash_cs_deselect();
}

uint32_t flash_read_jedec_id(void) {
    flash_cs_select();

    uint8_t tx = W25X_JEDEC_ID;
    uint8_t rx[3] = {0};

    spi_write_blocking(SPI_PORT_FLASH, &tx, 1);
    spi_read_blocking(SPI_PORT_FLASH, 0, rx, 3);

    flash_cs_deselect();

    return ((uint32_t)rx[0] << 16) | ((uint32_t)rx[1] << 8) | rx[2];
}

void flash_read(uint32_t addr, uint8_t* buffer, size_t len) {
    flash_cs_select();

    uint8_t cmd[4] = {
        W25X_READ_DATA,
        (addr >> 16) & 0xFF,
        (addr >> 8) & 0xFF,
        addr & 0xFF
    };

    spi_write_blocking(SPI_PORT_FLASH, cmd, 4);
    spi_read_blocking(SPI_PORT_FLASH, 0, buffer, len);

    flash_cs_deselect();
}

void flash_wait_busy(void) {
    flash_cs_select();

    uint8_t cmd = W25X_READ_STATUS_REG;
    uint8_t status;

    spi_write_blocking(SPI_PORT_FLASH, &cmd, 1);

    do {
        spi_read_blocking(SPI_PORT_FLASH, 0, &status, 1);
    } while (status & 0x01);  // BUSY bit

    flash_cs_deselect();
}

void flash_write_enable(void) {
    flash_cs_select();
    uint8_t cmd = W25X_WRITE_ENABLE;
    spi_write_blocking(SPI_PORT_FLASH, &cmd, 1);
    flash_cs_deselect();
}

void flash_sector_erase(uint32_t addr) {
    flash_write_enable();

    flash_cs_select();

    uint8_t cmd[4] = {
        W25X_SECTOR_ERASE,
        (addr >> 16) & 0xFF,
        (addr >> 8) & 0xFF,
        addr & 0xFF
    };

    spi_write_blocking(SPI_PORT_FLASH, cmd, 4);
    flash_cs_deselect();

    flash_wait_busy();
}

void flash_page_program(uint32_t addr, const uint8_t* data, size_t len) {
    if (len > 256) len = 256;  // 页大小限制

    flash_write_enable();

    flash_cs_select();

    uint8_t cmd[4] = {
        W25X_PAGE_PROGRAM,
        (addr >> 16) & 0xFF,
        (addr >> 8) & 0xFF,
        addr & 0xFF
    };

    spi_write_blocking(SPI_PORT_FLASH, cmd, 4);
    spi_write_blocking(SPI_PORT_FLASH, data, len);

    flash_cs_deselect();

    flash_wait_busy();
}

// ==================== TFT 显示屏操作 ====================

void tft_write_command(uint8_t cmd) {
    gpio_put(TFT_DC_PIN, 0);  // DC = 0 for command
    gpio_put(TFT_CS_PIN, 0);
    spi_write_blocking(SPI_PORT_TFT, &cmd, 1);
    gpio_put(TFT_CS_PIN, 1);
}

void tft_write_data(uint8_t data) {
    gpio_put(TFT_DC_PIN, 1);  // DC = 1 for data
    gpio_put(TFT_CS_PIN, 0);
    spi_write_blocking(SPI_PORT_TFT, &data, 1);
    gpio_put(TFT_CS_PIN, 1);
}

void tft_write_data16(uint16_t data) {
    gpio_put(TFT_DC_PIN, 1);
    gpio_put(TFT_CS_PIN, 0);
    uint8_t buf[2] = {data >> 8, data & 0xFF};
    spi_write_blocking(SPI_PORT_TFT, buf, 2);
    gpio_put(TFT_CS_PIN, 1);
}

void tft_init(void) {
    // 初始化 SPI
    spi_init(SPI_PORT_TFT, 40000000);  // 40MHz

    // 配置 GPIO
    gpio_set_function(TFT_MISO_PIN, GPIO_FUNC_SPI);
    gpio_set_function(TFT_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(TFT_SCK_PIN, GPIO_FUNC_SPI);

    // 控制引脚
    gpio_init(TFT_CS_PIN);
    gpio_init(TFT_DC_PIN);
    gpio_init(TFT_RST_PIN);
    gpio_set_dir(TFT_CS_PIN, GPIO_OUT);
    gpio_set_dir(TFT_DC_PIN, GPIO_OUT);
    gpio_set_dir(TFT_RST_PIN, GPIO_OUT);

    gpio_put(TFT_CS_PIN, 1);
    gpio_put(TFT_DC_PIN, 1);

    // 复位显示屏
    gpio_put(TFT_RST_PIN, 1);
    sleep_ms(5);
    gpio_put(TFT_RST_PIN, 0);
    sleep_ms(20);
    gpio_put(TFT_RST_PIN, 1);
    sleep_ms(150);

    // 初始化序列
    tft_write_command(ILI9341_SWRESET);
    sleep_ms(150);

    tft_write_command(ILI9341_SLPOUT);
    sleep_ms(500);

    tft_write_command(ILI9341_DISPON);
    sleep_ms(100);

    printf("TFT 初始化完成\n");
}

void tft_fill_screen(uint16_t color) {
    // 设置列地址
    tft_write_command(ILI9341_CASET);
    tft_write_data16(0);        // X start
    tft_write_data16(239);      // X end

    // 设置页地址
    tft_write_command(ILI9341_PASET);
    tft_write_data16(0);        // Y start
    tft_write_data16(319);      // Y end

    // 开始写入
    tft_write_command(ILI9341_RAMWR);

    // 批量写入
    gpio_put(TFT_DC_PIN, 1);
    gpio_put(TFT_CS_PIN, 0);

    for (int i = 0; i < 240 * 320; i++) {
        uint8_t buf[2] = {color >> 8, color & 0xFF};
        spi_write_blocking(SPI_PORT_TFT, buf, 2);
    }

    gpio_put(TFT_CS_PIN, 1);
}

int main(void) {
    stdio_init_all();
    sleep_ms(2000);

    printf("SPI 示例启动\n");

    // 初始化 Flash
    flash_init();
    uint32_t jedec_id = flash_read_jedec_id();
    printf("Flash JEDEC ID: 0x%06X\n", jedec_id);

    // Flash 读写测试
    uint8_t write_buf[256];
    uint8_t read_buf[256];

    for (int i = 0; i < 256; i++) {
        write_buf[i] = i;
    }

    printf("擦除扇区...\n");
    flash_sector_erase(0);

    printf("写入数据...\n");
    flash_page_program(0, write_buf, 256);

    printf("读取数据...\n");
    flash_read(0, read_buf, 256);

    // 验证
    bool match = true;
    for (int i = 0; i < 256; i++) {
        if (write_buf[i] != read_buf[i]) {
            match = false;
            break;
        }
    }
    printf("数据验证: %s\n", match ? "通过" : "失败");

    // 初始化 TFT
    tft_init();

    // 填充红色
    tft_fill_screen(0xF800);
    sleep_ms(1000);

    // 填充绿色
    tft_fill_screen(0x07E0);
    sleep_ms(1000);

    // 填充蓝色
    tft_fill_screen(0x001F);

    printf("完成!\n");

    while (1) {
        tight_loop_contents();
    }

    return 0;
}
