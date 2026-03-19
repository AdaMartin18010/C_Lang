/*
 * Auto-generated from: 03_System_Technology_Domains\03_Hardware_Acceleration.md
 * Line: 643
 * Language: c
 * Block ID: 9fd406c9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#include "stm32f4xx_hal.h"

DMA_HandleTypeDef hdma_uart_tx;
DMA_HandleTypeDef hdma_adc;

// UART TX DMA配置
void UART_TX_DMA_Init(UART_HandleTypeDef* huart) {
    __HAL_RCC_DMA2_CLK_ENABLE();

    hdma_uart_tx.Instance = DMA2_Stream7;
    hdma_uart_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_uart_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_uart_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_uart_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_uart_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_uart_tx.Init.Mode = DMA_NORMAL;
    hdma_uart_tx.Init.Priority = DMA_PRIORITY_HIGH;

    HAL_DMA_Init(&hdma_uart_tx);
    __HAL_LINKDMA(huart, hdmatx, hdma_uart_tx);

    // 启用中断
    HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);
}

// ADC DMA循环模式配置
void ADC_DMA_Init(ADC_HandleTypeDef* hadc, uint16_t* buffer, uint16_t size) {
    __HAL_RCC_DMA2_CLK_ENABLE();

    hdma_adc.Instance = DMA2_Stream0;
    hdma_adc.Init.Channel = DMA_CHANNEL_0;
    hdma_adc.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc.Init.Mode = DMA_CIRCULAR;  // 循环模式
    hdma_adc.Init.Priority = DMA_PRIORITY_VERY_HIGH;

    HAL_DMA_Init(&hdma_adc);
    __HAL_LINKDMA(hadc, DMA_Handle, hdma_adc);

    // 启动DMA传输
    HAL_ADC_Start_DMA(hadc, (uint32_t*)buffer, size);
}

// 双缓冲DMA（乒乓模式）
#define BUFFER_SIZE 1024
uint16_t buffer_a[BUFFER_SIZE];
uint16_t buffer_b[BUFFER_SIZE];

void ADC_DMA_DoubleBuffer_Init(ADC_HandleTypeDef* hadc) {
    // 配置DMA
    hdma_adc.Instance = DMA2_Stream0;
    hdma_adc.Init.Mode = DMA_CIRCULAR;
    // ... 其他配置

    HAL_DMA_Init(&hdma_adc);

    // 配置双缓冲
    HAL_DMAEx_MultiBufferStart(&hdma_adc,
                               (uint32_t)&hadc->Instance->DR,
                               (uint32_t)buffer_a,
                               (uint32_t)buffer_b,
                               BUFFER_SIZE);
}

// DMA传输完成回调
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    // buffer_a满，处理buffer_a，DMA正在写入buffer_b
    process_data(buffer_a, BUFFER_SIZE);
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
    // buffer_b满，处理buffer_b，DMA正在写入buffer_a
    process_data(buffer_b, BUFFER_SIZE);
}
