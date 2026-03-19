/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Multiplatform_Protocols.md
 * Line: 1853
 * Language: c
 * Block ID: b86fdbc7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// STM32 UART + DMA 环形缓冲区接收
// 高效处理高吞吐量数据流
// ============================================================

#define RX_BUFFER_SIZE 256

static uint8_t rx_buffer[RX_BUFFER_SIZE];
static volatile uint16_t rx_write_index = 0;
static volatile uint16_t rx_read_index = 0;

// 初始化UART DMA接收
void uart_dma_init(UART_HandleTypeDef *huart) {
    // 配置DMA循环模式
    HAL_UART_Receive_DMA(huart, rx_buffer, RX_BUFFER_SIZE);
}

// 获取已接收的数据长度
uint16_t uart_rx_available(void) {
    return (rx_write_index - rx_read_index) & (RX_BUFFER_SIZE - 1);
}

// DMA半传输完成回调
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart) {
    rx_write_index = RX_BUFFER_SIZE / 2;
}

// DMA传输完成回调
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    rx_write_index = RX_BUFFER_SIZE;
}

// 读取一个字节
bool uart_read(uint8_t *data) {
    if (rx_read_index == rx_write_index) {
        // 检查DMA当前位置 (处理DMA循环)
        uint16_t dma_pos = RX_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart->hdmarx);
        rx_write_index = dma_pos;

        if (rx_read_index == rx_write_index) {
            return false;  // 无数据
        }
    }

    *data = rx_buffer[rx_read_index & (RX_BUFFER_SIZE - 1)];
    rx_read_index++;
    return true;
}

// Modbus RTU over UART
bool modbus_rtu_receive(UART_HandleTypeDef *huart, uint8_t *frame,
                        uint16_t max_len, uint16_t *received_len,
                        uint32_t timeout_ms) {
    uint32_t start = HAL_GetTick();
    uint16_t idx = 0;
    uint32_t last_byte_time = start;

    while (idx < max_len) {
        uint8_t byte;
        if (uart_read(&byte)) {
            frame[idx++] = byte;
            last_byte_time = HAL_GetTick();
        }

        // Modbus 3.5字符时间间隔表示帧结束
        // 9600bps: 3.5 * 11bits = 4ms
        // 115200bps: 3.5 * 11bits = 0.3ms
        if (idx > 0 && (HAL_GetTick() - last_byte_time) > 5) {
            break;  // 帧间隔，认为帧结束
        }

        if (HAL_GetTick() - start > timeout_ms) {
            return false;  // 超时
        }
    }

    *received_len = idx;

    // CRC校验
    if (idx >= 2) {
        uint16_t crc = modbus_crc16(frame, idx - 2);
        uint16_t rx_crc = frame[idx-2] | (frame[idx-1] << 8);
        return (crc == rx_crc);
    }

    return false;
}
