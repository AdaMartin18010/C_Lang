/*
 * Auto-generated from: 03_System_Technology_Domains\05_Wireless_Protocol\02_LoRa_SX1276.md
 * Line: 176
 * Language: c
 * Block ID: cbe12860
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* SPI接口抽象 */
typedef struct {
    void (*spi_select)(void);      /* CS拉低 */
    void (*spi_deselect)(void);    /* CS拉高 */
    uint8_t (*spi_transfer)(uint8_t data);  /* 全双工传输 */
    void (*delay_ms)(uint32_t ms);
    void (*delay_us)(uint32_t us);
} SX1276_HAL;

/* 全局HAL实例 */
static SX1276_HAL *hal;

/* SPI单字节读取 */
static uint8_t sx1276_read_reg(uint8_t reg) {
    hal->spi_select();
    hal->spi_transfer(reg & 0x7F);  /* 读操作: MSB=0 */
    uint8_t val = hal->spi_transfer(0x00);
    hal->spi_deselect();
    return val;
}

/* SPI单字节写入 */
static void sx1276_write_reg(uint8_t reg, uint8_t val) {
    hal->spi_select();
    hal->spi_transfer(reg | 0x80);  /* 写操作: MSB=1 */
    hal->spi_transfer(val);
    hal->spi_deselect();
}

/* 突发读取 */
static void sx1276_read_burst(uint8_t reg, uint8_t *buffer, uint8_t len) {
    hal->spi_select();
    hal->spi_transfer(reg & 0x7F);
    for (uint8_t i = 0; i < len; i++) {
        buffer[i] = hal->spi_transfer(0x00);
    }
    hal->spi_deselect();
}

/* 突发写入 */
static void sx1276_write_burst(uint8_t reg, const uint8_t *buffer, uint8_t len) {
    hal->spi_select();
    hal->spi_transfer(reg | 0x80);
    for (uint8_t i = 0; i < len; i++) {
        hal->spi_transfer(buffer[i]);
    }
    hal->spi_deselect();
}
