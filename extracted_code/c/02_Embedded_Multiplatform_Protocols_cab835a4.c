/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Multiplatform_Protocols.md
 * Line: 1486
 * Language: c
 * Block ID: cab835a4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 软件SPI (Bit-Bang) 实现
// 支持4种模式
// ============================================================

typedef struct {
    uint32_t mosi_pin;
    uint32_t miso_pin;
    uint32_t sck_pin;
    uint32_t cs_pin;
    uint8_t mode;  // 0-3
} SoftSPI;

// 初始化软件SPI
void softspi_init(SoftSPI *spi, uint8_t mode) {
    spi->mode = mode & 0x03;

    // 配置引脚
    gpio_init(spi->mosi_pin);
    gpio_init(spi->miso_pin);
    gpio_init(spi->sck_pin);
    gpio_init(spi->cs_pin);

    gpio_set_dir(spi->mosi_pin, GPIO_OUT);
    gpio_set_dir(spi->miso_pin, GPIO_IN);
    gpio_set_dir(spi->sck_pin, GPIO_OUT);
    gpio_set_dir(spi->cs_pin, GPIO_OUT);

    // 设置初始状态
    gpio_put(spi->cs_pin, 1);  // CS高 (未选中)

    // 根据CPOL设置初始时钟状态
    uint8_t cpol = (spi->mode >> 1) & 1;
    gpio_put(spi->sck_pin, cpol);
}

// 延时
static inline void spi_delay(void) {
    for (volatile int i = 0; i < 5; i++);
}

// 发送接收一个字节
uint8_t softspi_transfer(SoftSPI *spi, uint8_t data) {
    uint8_t received = 0;
    uint8_t cpol = (spi->mode >> 1) & 1;
    uint8_t cpha = spi->mode & 1;

    for (int i = 7; i >= 0; i--) {
        // 准备数据 (MSB first)
        if (data & (1 << i)) {
            gpio_put(spi->mosi_pin, 1);
        } else {
            gpio_put(spi->mosi_pin, 0);
        }

        if (cpha == 0) {
            // 模式0,2: 第一个边沿采样
            spi_delay();
            gpio_put(spi->sck_pin, !cpol);  // 时钟翻转
            spi_delay();
            received = (received << 1) | gpio_get(spi->miso_pin);
            gpio_put(spi->sck_pin, cpol);   // 时钟恢复
        } else {
            // 模式1,3: 第二个边沿采样
            spi_delay();
            gpio_put(spi->sck_pin, !cpol);  // 时钟翻转 (数据变化)
            spi_delay();
            gpio_put(spi->sck_pin, cpol);   // 时钟恢复 (采样)
            spi_delay();
            received = (received << 1) | gpio_get(spi->miso_pin);
        }
    }

    return received;
}

// CS控制
void softspi_begin(SoftSPI *spi) {
    gpio_put(spi->cs_pin, 0);
    spi_delay();
}

void softspi_end(SoftSPI *spi) {
    spi_delay();
    gpio_put(spi->cs_pin, 1);
}

// 发送数据
void softspi_write(SoftSPI *spi, const uint8_t *data, size_t len) {
    softspi_begin(spi);
    for (size_t i = 0; i < len; i++) {
        softspi_transfer(spi, data[i]);
    }
    softspi_end(spi);
}

// 接收数据
void softspi_read(SoftSPI *spi, uint8_t *data, size_t len) {
    softspi_begin(spi);
    for (size_t i = 0; i < len; i++) {
        data[i] = softspi_transfer(spi, 0xFF);  // 发送0xFF，读取MISO
    }
    softspi_end(spi);
}
