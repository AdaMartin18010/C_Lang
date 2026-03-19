/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\ESP32\01_ESP32_Complete_Guide.md
 * Line: 3061
 * Language: c
 * Block ID: feae6feb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * uart_driver_example.c
 * ESP32 UART 完整示例
 * 支持: UART0(调试), UART1, UART2
 * 功能: 标准通信, RS485, 红外 (IrDA), 唤醒
 */

#include "driver/uart.h"
#include "string.h"

#define UART_PORT_NUM           UART_NUM_2
#define UART_TX_PIN             GPIO_NUM_17
#define UART_RX_PIN             GPIO_NUM_16
#define UART_RTS_PIN            GPIO_NUM_15
#define UART_CTS_PIN            GPIO_NUM_14
#define UART_BAUD_RATE          115200
#define UART_BUF_SIZE           1024

/**
 * @brief 初始化 UART
 */
static void uart_init(void)
{
    /* UART 配置 */
    const uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = 122,
        .source_clk = UART_SCLK_DEFAULT,
    };

    /* 安装驱动 */
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM,
                                        UART_BUF_SIZE * 2,
                                        UART_BUF_SIZE * 2,
                                        20, NULL, 0));

    /* 配置参数 */
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));

    /* 设置引脚 */
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM,
                                 UART_TX_PIN,
                                 UART_RX_PIN,
                                 UART_RTS_PIN,
                                 UART_CTS_PIN));
}

/**
 * @brief UART 发送数据
 */
static void uart_send(const uint8_t *data, size_t len)
{
    uart_write_bytes(UART_PORT_NUM, data, len);
}

/**
 * @brief UART 接收任务
 */
static void uart_rx_task(void *pvParameters)
{
    uint8_t data[UART_BUF_SIZE];

    while (1) {
        /* 读取数据 */
        int len = uart_read_bytes(UART_PORT_NUM, data,
                                  (UART_BUF_SIZE - 1),
                                  pdMS_TO_TICKS(100));

        if (len > 0) {
            data[len] = '\0';
            ESP_LOGI(TAG, "UART 接收 %d 字节: %s", len, data);

            /* 回显 */
            uart_send(data, len);
        }
    }
}

/**
 * @brief UART 模式检测 (自动波特率)
 */
static void uart_auto_baudrate_detect(void)
{
    /* 启用自动波特率检测 */
    ESP_ERROR_CHECK(uart_set_mode(UART_PORT_NUM, UART_MODE_RS485_HALF_DUPLEX));

    /* 或启用红外模式 */
    /* ESP_ERROR_CHECK(uart_set_mode(UART_PORT_NUM, UART_MODE_IRDA)); */
}

/**
 * @brief RS485 半双工模式配置
 */
static void uart_rs485_config(void)
{
    /* 配置 RS485 */
    ESP_ERROR_CHECK(uart_set_mode(UART_PORT_NUM, UART_MODE_RS485_HALF_DUPLEX));

    /* 配置 RS485 特定的冲突检测 */
    ESP_ERROR_CHECK(uart_set_rx_timeout(UART_PORT_NUM, 3));

    /* 设置 RTS 有效电平 (用于方向控制) */
    ESP_ERROR_CHECK(uart_set_line_inverse(UART_PORT_NUM, UART_SIGNAL_RTS_INV));
}

/**
 * @brief UART 事件处理
 */
static void uart_event_task(void *pvParameters)
{
    uart_event_t event;

    while (1) {
        /* 等待 UART 事件 */
        if (xQueueReceive(uart_queue, (void *)&event, portMAX_DELAY)) {
            switch (event.type) {
            case UART_DATA:
                ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
                break;

            case UART_FIFO_OVF:
                ESP_LOGI(TAG, "[UART FIFO 溢出]");
                uart_flush_input(UART_PORT_NUM);
                break;

            case UART_BUFFER_FULL:
                ESP_LOGI(TAG, "[UART 缓冲区满]");
                uart_flush_input(UART_PORT_NUM);
                break;

            case UART_BREAK:
                ESP_LOGI(TAG, "[UART BREAK]");
                break;

            case UART_PARITY_ERR:
                ESP_LOGI(TAG, "[UART 奇偶校验错误]");
                break;

            case UART_FRAME_ERR:
                ESP_LOGI(TAG, "[UART 帧错误]");
                break;

            default:
                ESP_LOGI(TAG, "[UART 事件类型]: %d", event.type);
                break;
            }
        }
    }
}
