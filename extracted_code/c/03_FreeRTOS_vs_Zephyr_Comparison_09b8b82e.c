/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\03_FreeRTOS_vs_Zephyr_Comparison.md
 * Line: 4222
 * Language: c
 * Block ID: 09b8b82e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * FreeRTOS 设备访问示例（无标准驱动框架）
 * 文件: freertos_device_example.c
 * 说明: 展示典型的FreeRTOS设备访问模式
 */

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdio.h>
#include <string.h>

/*
 * 自定义HAL层 - UART驱动示例
 * 实际项目中通常由厂商SDK提供
 */

/* UART配置结构 */
typedef struct {
    uint32_t baudrate;
    uint8_t data_bits;
    uint8_t stop_bits;
    uint8_t parity;
    void *instance;  /* 硬件寄存器基地址 */
} UART_Config_t;

/* UART句柄 */
typedef struct {
    UART_Config_t config;
    uint8_t *rx_buffer;
    uint16_t rx_size;
    volatile uint16_t rx_count;
    SemaphoreHandle_t rx_sem;
    SemaphoreHandle_t tx_mutex;
} UART_Handle_t;

/*
 * UART驱动实现
 */
static void UART_Init(UART_Handle_t *huart, UART_Config_t *config)
{
    memcpy(&huart->config, config, sizeof(UART_Config_t));
    huart->rx_count = 0;
    huart->rx_sem = xSemaphoreCreateBinary();
    huart->tx_mutex = xSemaphoreCreateMutex();

    /* 硬件初始化 */
    /* HAL_UART_Init(huart); */
}

static int UART_Transmit(UART_Handle_t *huart, const uint8_t *data, uint16_t len, uint32_t timeout)
{
    if (xSemaphoreTake(huart->tx_mutex, pdMS_TO_TICKS(timeout)) != pdTRUE) {
        return -1;  /* 超时 */
    }

    /* 发送数据 */
    for (uint16_t i = 0; i < len; i++) {
        /* 等待发送完成 */
        /* while (!(huart->instance->SR & USART_SR_TXE)); */
        /* huart->instance->DR = data[i]; */
    }

    xSemaphoreGive(huart->tx_mutex);
    return 0;
}

static int UART_Receive_IT(UART_Handle_t *huart, uint8_t *buffer, uint16_t size)
{
    huart->rx_buffer = buffer;
    huart->rx_size = size;
    huart->rx_count = 0;

    /* 使能中断接收 */
    /* huart->instance->CR1 |= USART_CR1_RXNEIE; */

    return 0;
}

/* UART中断处理 */
void USART_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    extern UART_Handle_t huart1;

    /* 读取接收数据 */
    /* uint8_t data = huart1.instance->DR; */
    uint8_t data = 0;

    if (huart1.rx_count < huart1.rx_size) {
        huart1.rx_buffer[huart1.rx_count++] = data;

        if (huart1.rx_count >= huart1.rx_size) {
            xSemaphoreGiveFromISR(huart1.rx_sem, &xHigherPriorityTaskWoken);
        }
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/*
 * GPIO驱动示例
 */
typedef enum {
    GPIO_MODE_INPUT = 0,
    GPIO_MODE_OUTPUT,
    GPIO_MODE_AF,
    GPIO_MODE_ANALOG
} GPIO_Mode_t;

typedef struct {
    uint16_t pin;
    GPIO_Mode_t mode;
    uint8_t pull;
    uint8_t speed;
} GPIO_Config_t;

static void GPIO_Init(uint16_t pin, GPIO_Config_t *config)
{
    /* HAL_GPIO_Init(pin, config); */
    (void)pin;
    (void)config;
}

static void GPIO_WritePin(uint16_t pin, uint8_t value)
{
    /* HAL_GPIO_WritePin(pin, value); */
    (void)pin;
    (void)value;
}

static uint8_t GPIO_ReadPin(uint16_t pin)
{
    /* return HAL_GPIO_ReadPin(pin); */
    (void)pin;
    return 0;
}

/*
 * SPI驱动示例
 */
typedef struct {
    void *instance;
    SemaphoreHandle_t mutex;
    SemaphoreHandle_t complete_sem;
} SPI_Handle_t;

static void SPI_Init(SPI_Handle_t *hspi)
{
    hspi->mutex = xSemaphoreCreateMutex();
    hspi->complete_sem = xSemaphoreCreateBinary();
}

static int SPI_TransmitReceive(SPI_Handle_t *hspi, uint8_t *tx, uint8_t *rx,
                                uint16_t len, uint32_t timeout)
{
    if (xSemaphoreTake(hspi->mutex, pdMS_TO_TICKS(timeout)) != pdTRUE) {
        return -1;
    }

    /* 启动SPI传输 */
    /* HAL_SPI_TransmitReceive_IT(hspi, tx, rx, len); */

    /* 等待完成 */
    if (xSemaphoreTake(hspi->complete_sem, pdMS_TO_TICKS(timeout)) != pdTRUE) {
        xSemaphoreGive(hspi->mutex);
        return -1;
    }

    xSemaphoreGive(hspi->mutex);
    return 0;
}

/*
 * 传感器驱动 - 基于上述HAL
 */
typedef struct {
    SPI_Handle_t *hspi;
    uint16_t cs_pin;
    float last_reading;
} Sensor_Driver_t;

static int Sensor_Read(Sensor_Driver_t *sensor, float *value)
{
    uint8_t tx_buf[4] = {0x01, 0x80, 0x00, 0x00};  /* 读取命令 */
    uint8_t rx_buf[4] = {0};

    /* 片选拉低 */
    GPIO_WritePin(sensor->cs_pin, 0);

    /* SPI传输 */
    int ret = SPI_TransmitReceive(sensor->hspi, tx_buf, rx_buf, 4, 100);

    /* 片选拉高 */
    GPIO_WritePin(sensor->cs_pin, 1);

    if (ret == 0) {
        /* 解析数据 */
        uint16_t raw = ((rx_buf[2] << 8) | rx_buf[3]);
        *value = (float)raw * 0.01f;
        sensor->last_reading = *value;
    }

    return ret;
}

/*
 * 应用任务
 */
static UART_Handle_t huart1;
static SPI_Handle_t hspi1;
static Sensor_Driver_t temp_sensor;

static void vSensorTask(void *pvParameters)
{
    (void)pvParameters;
    float temperature;

    printf("[SensorTask] Started\n");

    /* 初始化传感器 */
    temp_sensor.hspi = &hspi1;
    temp_sensor.cs_pin = 0;  /* GPIO pin number */

    for (;;) {
        if (Sensor_Read(&temp_sensor, &temperature) == 0) {
            printf("[SensorTask] Temperature: %.2f C\n", temperature);
        } else {
            printf("[SensorTask] Sensor read failed\n");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void vCommunicationTask(void *pvParameters)
{
    (void)pvParameters;
    uint8_t tx_buffer[64];

    printf("[CommTask] Started\n");

    for (;;) {
        /* 准备数据 */
        snprintf((char*)tx_buffer, 64, "Temp: %.2f C\r\n",
                 temp_sensor.last_reading);

        /* 发送 */
        UART_Transmit(&huart1, tx_buffer, strlen((char*)tx_buffer), 1000);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

int main(void)
{
    printf("\n=== FreeRTOS Device Access Demo ===\n\n");

    /* 初始化硬件 */
    UART_Config_t uart_config = {
        .baudrate = 115200,
        .data_bits = 8,
        .stop_bits = 1,
        .parity = 0,
        .instance = NULL
    };
    UART_Init(&huart1, &uart_config);

    SPI_Init(&hspi1);

    /* 创建任务 */
    xTaskCreate(vSensorTask, "Sensor", 512, NULL, 2, NULL);
    xTaskCreate(vCommunicationTask, "Comm", 512, NULL, 1, NULL);

    vTaskStartScheduler();

    return 0;
}
