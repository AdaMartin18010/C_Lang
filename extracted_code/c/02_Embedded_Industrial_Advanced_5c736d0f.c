/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Industrial_Advanced.md
 * Line: 1357
 * Language: c
 * Block ID: 5c736d0f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 跨平台HAL设计
// 目标: 同一应用代码可在STM32/ESP32/RP2040上编译运行
// ============================================================

// ========== 抽象接口层 ==========

// GPIO抽象
typedef struct {
    int (*init)(void *config);
    int (*set_mode)(uint32_t pin, uint32_t mode);
    int (*write)(uint32_t pin, uint32_t value);
    int (*read)(uint32_t pin);
    int (*toggle)(uint32_t pin);
} gpio_driver_t;

// UART抽象
typedef struct {
    int (*init)(uint32_t baudrate);
    int (*send)(const uint8_t *data, size_t len);
    int (*receive)(uint8_t *data, size_t len, uint32_t timeout);
    int (*printf)(const char *fmt, ...);
} uart_driver_t;

// I2C抽象
typedef struct {
    int (*init)(uint32_t freq);
    int (*write)(uint8_t addr, const uint8_t *data, size_t len);
    int (*read)(uint8_t addr, uint8_t *data, size_t len);
    int (*write_reg)(uint8_t addr, uint8_t reg, uint8_t value);
    int (*read_reg)(uint8_t addr, uint8_t reg, uint8_t *value);
} i2c_driver_t;

// SPI抽象
typedef struct {
    int (*init)(uint32_t freq, uint32_t mode);
    int (*transfer)(const uint8_t *tx, uint8_t *rx, size_t len);
    int (*send)(const uint8_t *data, size_t len);
    int (*receive)(uint8_t *data, size_t len);
} spi_driver_t;

// 驱动注册表
typedef struct {
    const char *name;
    gpio_driver_t *gpio;
    uart_driver_t *uart;
    i2c_driver_t *i2c;
    spi_driver_t *spi;
} platform_drivers_t;

// ========== 平台具体实现 ==========

#ifdef PLATFORM_STM32

static int stm32_gpio_init(void *config) {
    // STM32 GPIO初始化代码
    __HAL_RCC_GPIOA_CLK_ENABLE();
    return 0;
}

static int stm32_gpio_write(uint32_t pin, uint32_t value) {
    HAL_GPIO_WritePin(GPIOA, pin, value ? GPIO_PIN_SET : GPIO_PIN_RESET);
    return 0;
}

// ... 其他STM32实现

gpio_driver_t stm32_gpio_driver = {
    .init = stm32_gpio_init,
    .write = stm32_gpio_write,
    // ...
};

platform_drivers_t stm32_platform = {
    .name = "STM32F4",
    .gpio = &stm32_gpio_driver,
    // ...
};

#elif defined(PLATFORM_ESP32)

static int esp32_gpio_init(void *config) {
    gpio_install_isr_service(0);
    return 0;
}

static int esp32_gpio_write(uint32_t pin, uint32_t value) {
    return gpio_set_level(pin, value);
}

gpio_driver_t esp32_gpio_driver = {
    .init = esp32_gpio_init,
    .write = esp32_gpio_write,
    // ...
};

platform_drivers_t esp32_platform = {
    .name = "ESP32",
    .gpio = &esp32_gpio_driver,
    // ...
};

#elif defined(PLATFORM_RP2040)

static int rp2040_gpio_init(void *config) {
    // RP2040 GPIO初始化
    return 0;
}

static int rp2040_gpio_write(uint32_t pin, uint32_t value) {
    gpio_put(pin, value);
    return 0;
}

gpio_driver_t rp2040_gpio_driver = {
    .init = rp2040_gpio_init,
    .write = rp2040_gpio_write,
    // ...
};

platform_drivers_t rp2040_platform = {
    .name = "RP2040",
    .gpio = &rp2040_gpio_driver,
    // ...
};

#endif

// ========== 运行时平台选择 ==========

static platform_drivers_t *current_platform = NULL;

int platform_init(const char *platform_name) {
    #ifdef PLATFORM_STM32
    if (strcmp(platform_name, "STM32") == 0) {
        current_platform = &stm32_platform;
    }
    #elif defined(PLATFORM_ESP32)
    if (strcmp(platform_name, "ESP32") == 0) {
        current_platform = &esp32_platform;
    }
    #elif defined(PLATFORM_RP2040)
    if (strcmp(platform_name, "RP2040") == 0) {
        current_platform = &rp2040_platform;
    }
    #endif

    if (current_platform == NULL) {
        return -1;  // 不支持的平台
    }

    // 初始化所有驱动
    current_platform->gpio->init(NULL);
    // ...

    return 0;
}

// 统一API
int gpio_write(uint32_t pin, uint32_t value) {
    if (current_platform && current_platform->gpio) {
        return current_platform->gpio->write(pin, value);
    }
    return -1;
}

int uart_send(const uint8_t *data, size_t len) {
    if (current_platform && current_platform->uart) {
        return current_platform->uart->send(data, len);
    }
    return -1;
}
