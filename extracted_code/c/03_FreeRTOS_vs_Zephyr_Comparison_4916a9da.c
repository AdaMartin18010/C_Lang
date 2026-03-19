/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\03_FreeRTOS_vs_Zephyr_Comparison.md
 * Line: 4505
 * Language: c
 * Block ID: 4916a9da
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Zephyr RTOS 设备驱动示例
 * 文件: zephyr_device_example.c
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(device_example, LOG_LEVEL_DBG);

/*
 * 设备树获取设备
 * 设备在设备树中定义，编译时绑定
 */

/* UART设备 */
#define UART_DEV_NAME   DT_LABEL(DT_ALIAS(uart_0))

/* SPI设备 */
#define SPI_DEV_NAME    DT_LABEL(DT_ALIAS(spi_0))

/* GPIO设备 */
#define LED0_NODE       DT_ALIAS(led0)
#define SW0_NODE        DT_ALIAS(sw0)

/* 传感器设备 */
#define TEMP_SENSOR_NODE    DT_INST(0, vendor_temperature_sensor)

/*
 * 设备句柄 - 运行时获取
 */
static const struct device *uart_dev;
static const struct device *spi_dev;
static const struct device *sensor_dev;

/* GPIO设备（LED和按钮） */
#if DT_NODE_HAS_STATUS(LED0_NODE, okay)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
#endif

#if DT_NODE_HAS_STATUS(SW0_NODE, okay)
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW0_NODE, gpios);
static struct gpio_callback button_cb;
#endif

/*
 * UART异步回调
 */
static void uart_callback(const struct device *dev, struct uart_event *evt,
                          void *user_data)
{
    ARG_UNUSED(dev);
    ARG_UNUSED(user_data);

    switch (evt->type) {
        case UART_TX_DONE:
            LOG_DBG("[UART] TX complete");
            break;

        case UART_RX_RDY:
            LOG_DBG("[UART] RX ready, len=%zu", evt->data.rx.len);
            break;

        case UART_RX_BUF_REQUEST:
            LOG_DBG("[UART] RX buffer request");
            break;

        case UART_RX_BUF_RELEASED:
            LOG_DBG("[UART] RX buffer released");
            break;

        case UART_RX_DISABLED:
            LOG_DBG("[UART] RX disabled");
            break;

        default:
            break;
    }
}

/*
 * UART通信函数
 */
static int uart_send(const uint8_t *data, size_t len)
{
    /* 使用异步API */
    int ret = uart_tx(uart_dev, data, len, SYS_FOREVER_MS);
    return ret;
}

static int uart_receive_async(uint8_t *buffer, size_t len)
{
    /* 启动异步接收 */
    int ret = uart_rx_enable(uart_dev, buffer, len, 1000);
    return ret;
}

static int uart_receive_poll(uint8_t *buffer, size_t len)
{
    /* 轮询接收 */
    for (size_t i = 0; i < len; i++) {
        uint8_t c;
        int ret = uart_poll_in(uart_dev, &c);
        if (ret < 0) {
            return i;  /* 返回已接收字节数 */
        }
        buffer[i] = c;
    }
    return len;
}

/*
 * SPI通信函数
 */
static int spi_transfer(const uint8_t *tx_buf, uint8_t *rx_buf, size_t len)
{
    struct spi_config config;
    struct spi_buf tx_spi_buf = {
        .buf = (uint8_t *)tx_buf,
        .len = len
    };
    struct spi_buf rx_spi_buf = {
        .buf = rx_buf,
        .len = len
    };
    struct spi_buf_set tx_set = {
        .buffers = &tx_spi_buf,
        .count = 1
    };
    struct spi_buf_set rx_set = {
        .buffers = &rx_spi_buf,
        .count = 1
    };

    /* 配置SPI */
    config.frequency = 1000000;  /* 1MHz */
    config.operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8);
    config.slave = 0;
    config.cs = NULL;  /* 软件控制CS */

    /* 传输 */
    int ret = spi_transceive(spi_dev, &config, &tx_set, &rx_set);
    return ret;
}

/*
 * GPIO回调
 */
#if DT_NODE_HAS_STATUS(SW0_NODE, okay)
static void button_isr(const struct device *dev, struct gpio_callback *cb,
                       uint32_t pins)
{
    ARG_UNUSED(dev);
    ARG_UNUSED(cb);
    ARG_UNUSED(pins);

    LOG_INF("[GPIO] Button pressed!");
}
#endif

static void led_toggle(void)
{
    #if DT_NODE_HAS_STATUS(LED0_NODE, okay)
    static bool led_state = false;
    led_state = !led_state;
    gpio_pin_set_dt(&led, led_state);
    LOG_DBG("[GPIO] LED toggled: %s", led_state ? "ON" : "OFF");
    #endif
}

/*
 * 传感器读取
 */
#if DT_NODE_HAS_STATUS(TEMP_SENSOR_NODE, okay)
static int read_temperature(double *temperature)
{
    struct sensor_value temp_val;
    int ret;

    /* 获取传感器样本 */
    ret = sensor_sample_fetch(sensor_dev);
    if (ret != 0) {
        LOG_ERR("[Sensor] Failed to fetch sample: %d", ret);
        return ret;
    }

    /* 获取温度通道 */
    ret = sensor_channel_get(sensor_dev, SENSOR_CHAN_AMBIENT_TEMP, &temp_val);
    if (ret != 0) {
        LOG_ERR("[Sensor] Failed to get channel: %d", ret);
        return ret;
    }

    /* 转换为浮点 */
    *temperature = sensor_value_to_double(&temp_val);
    return 0;
}
#endif

/*
 * 传感器任务
 */
static void sensor_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    double temperature;

    LOG_INF("[SensorThread] Started");

    while (1) {
        #if DT_NODE_HAS_STATUS(TEMP_SENSOR_NODE, okay)
        if (read_temperature(&temperature) == 0) {
            LOG_INF("[SensorThread] Temperature: %.2f C", temperature);
        }
        #else
        /* 模拟读取 */
        temperature = 25.0 + (k_uptime_get_32() % 10);
        LOG_INF("[SensorThread] Simulated temperature: %.2f C", temperature);
        #endif

        k_sleep(K_SECONDS(1));
    }
}

/*
 * 通信任务
 */
static void communication_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    uint8_t tx_buffer[64];

    LOG_INF("[CommThread] Started");

    while (1) {
        /* 准备数据 */
        snprintf((char*)tx_buffer, sizeof(tx_buffer),
                 "Hello from Zephyr! Uptime: %llu ms\r\n", k_uptime_get());

        /* 发送 */
        if (uart_dev != NULL) {
            uart_send(tx_buffer, strlen((char*)tx_buffer));
        }

        /* 切换LED */
        led_toggle();

        k_sleep(K_SECONDS(2));
    }
}

/* 线程栈 */
K_THREAD_STACK_DEFINE(sensor_thread_stack, 2048);
K_THREAD_STACK_DEFINE(comm_thread_stack, 2048);

static struct k_thread sensor_thread_data;
static struct k_thread comm_thread_data;

/*
 * 设备初始化
 */
static int init_devices(void)
{
    int ret;

    /* 获取UART设备 */
    uart_dev = DEVICE_DT_GET(DT_ALIAS(uart_0));
    if (!device_is_ready(uart_dev)) {
        LOG_ERR("UART device not ready");
        return -ENODEV;
    }

    /* 注册UART回调 */
    ret = uart_callback_set(uart_dev, uart_callback, NULL);
    if (ret != 0) {
        LOG_ERR("Failed to set UART callback: %d", ret);
    }

    /* 获取SPI设备 */
    spi_dev = DEVICE_DT_GET(DT_ALIAS(spi_0));
    if (!device_is_ready(spi_dev)) {
        LOG_WRN("SPI device not ready");
        spi_dev = NULL;
    }

    /* 获取传感器设备 */
    #if DT_NODE_HAS_STATUS(TEMP_SENSOR_NODE, okay)
    sensor_dev = DEVICE_DT_GET(TEMP_SENSOR_NODE);
    if (!device_is_ready(sensor_dev)) {
        LOG_WRN("Sensor device not ready");
        sensor_dev = NULL;
    }
    #endif

    /* 配置GPIO */
    #if DT_NODE_HAS_STATUS(LED0_NODE, okay)
    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
    if (ret != 0) {
        LOG_ERR("Failed to configure LED: %d", ret);
    }
    #endif

    #if DT_NODE_HAS_STATUS(SW0_NODE, okay)
    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (ret != 0) {
        LOG_ERR("Failed to configure button: %d", ret);
    } else {
        ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
        if (ret != 0) {
            LOG_ERR("Failed to configure button interrupt: %d", ret);
        } else {
            gpio_init_callback(&button_cb, button_isr, BIT(button.pin));
            gpio_add_callback(button.port, &button_cb);
        }
    }
    #endif

    LOG_INF("Devices initialized");
    return 0;
}

int main(void)
{
    printk("\n=== Zephyr RTOS Device Driver Demo ===\n\n");

    /* 初始化设备 */
    if (init_devices() != 0) {
        LOG_ERR("Device initialization failed");
    }

    /* 创建任务 */
    k_thread_create(&sensor_thread_data, sensor_thread_stack,
                    K_THREAD_STACK_SIZEOF(sensor_thread_stack),
                    sensor_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(5), 0, K_NO_WAIT);

    k_thread_create(&comm_thread_data, comm_thread_stack,
                    K_THREAD_STACK_SIZEOF(comm_thread_stack),
                    communication_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(4), 0, K_NO_WAIT);

    /* 主循环 */
    while (1) {
        k_sleep(K_SECONDS(30));
    }

    return 0;
}
