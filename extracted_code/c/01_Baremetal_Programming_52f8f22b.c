/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Baremetal\01_Baremetal_Programming.md
 * Line: 2559
 * Language: c
 * Block ID: 52f8f22b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file uart_driver.c
 * @brief STM32F103 USART驱动程序
 * @details 提供轮询和中断两种模式的串口通信
 */

#include <stdint.h>

/* ============================================================================
 * USART 寄存器定义
 * ============================================================================ */

typedef struct {
    volatile uint32_t SR;       /* 状态寄存器 (0x00) */
    volatile uint32_t DR;       /* 数据寄存器 (0x04) */
    volatile uint32_t BRR;      /* 波特率寄存器 (0x08) */
    volatile uint32_t CR1;      /* 控制寄存器1 (0x0C) */
    volatile uint32_t CR2;      /* 控制寄存器2 (0x10) */
    volatile uint32_t CR3;      /* 控制寄存器3 (0x14) */
    volatile uint32_t GTPR;     /* 保护时间和预分频寄存器 (0x18) */
} USART_TypeDef;

/* USART 基地址 */
#define USART1_BASE         0x40013800
#define USART2_BASE         0x40004400
#define USART3_BASE         0x40004800

#define USART1              ((USART_TypeDef *)USART1_BASE)
#define USART2              ((USART_TypeDef *)USART2_BASE)
#define USART3              ((USART_TypeDef *)USART3_BASE)

/* ============================================================================
 * 寄存器位定义
 * ============================================================================ */

/* SR - 状态寄存器 */
#define USART_SR_PE_Pos     0   /* 奇偶校验错误 */
#define USART_SR_FE_Pos     1   /* 帧错误 */
#define USART_SR_NE_Pos     2   /* 噪声错误 */
#define USART_SR_ORE_Pos    3   /* 过载错误 */
#define USART_SR_IDLE_Pos   4   /* 空闲线路 */
#define USART_SR_RXNE_Pos   5   /* 接收数据寄存器非空 */
#define USART_SR_TC_Pos     6   /* 发送完成 */
#define USART_SR_TXE_Pos    7   /* 发送数据寄存器空 */
#define USART_SR_LBD_Pos    8   /* LIN断开检测 */
#define USART_SR_CTS_Pos    9   /* CTS标志 */

/* CR1 - 控制寄存器1 */
#define USART_CR1_SBK_Pos   0   /* 发送断开 */
#define USART_CR1_RWU_Pos   1   /* 接收唤醒 */
#define USART_CR1_RE_Pos    2   /* 接收使能 */
#define USART_CR1_TE_Pos    3   /* 发送使能 */
#define USART_CR1_IDLEIE_Pos 4  /* 空闲中断使能 */
#define USART_CR1_RXNEIE_Pos 5  /* RXNE中断使能 */
#define USART_CR1_TCIE_Pos  6   /* 发送完成中断使能 */
#define USART_CR1_TXEIE_Pos 7   /* TXE中断使能 */
#define USART_CR1_PEIE_Pos  8   /* PE中断使能 */
#define USART_CR1_PS_Pos    9   /* 奇偶校验选择 */
#define USART_CR1_PCE_Pos   10  /* 奇偶校验使能 */
#define USART_CR1_WAKE_Pos  11  /* 唤醒方法 */
#define USART_CR1_M_Pos     12  /* 字长 */
#define USART_CR1_UE_Pos    13  /* USART使能 */

/* CR2 - 控制寄存器2 */
#define USART_CR2_ADD_Pos   0   /* 地址 */
#define USART_CR2_LBDL_Pos  5   /* LIN断开长度 */
#define USART_CR2_LBDIE_Pos 6   /* LIN断开中断使能 */
#define USART_CR2_LBCL_Pos  8   /* 最后一位时钟脉冲 */
#define USART_CR2_CPHA_Pos  9   /* 时钟相位 */
#define USART_CR2_CPOL_Pos  10  /* 时钟极性 */
#define USART_CR2_CLKEN_Pos 11  /* 时钟使能 */
#define USART_CR2_STOP_Pos  12  /* 停止位 */

/* CR3 - 控制寄存器3 */
#define USART_CR3_EIE_Pos   0   /* 错误中断使能 */
#define USART_CR3_IREN_Pos  1   /* 红外模式 */
#define USART_CR3_IRLP_Pos  2   /* 红外低功耗 */
#define USART_CR3_HDSEL_Pos 3   /* 半双工选择 */
#define USART_CR3_NACK_Pos  4   /* 智能卡NACK */
#define USART_CR3_SCEN_Pos  5   /* 智能卡模式 */
#define USART_CR3_DMAR_Pos  6   /* DMA接收 */
#define USART_CR3_DMAT_Pos  7   /* DMA发送 */
#define USART_CR3_RTSE_Pos  8   /* RTS使能 */
#define USART_CR3_CTSE_Pos  9   /* CTS使能 */
#define USART_CR3_CTSIE_Pos 10  /* CTS中断使能 */

/* ============================================================================
 * 波特率计算
 * ============================================================================ */

/* APB2时钟频率 (USART1) */
#define USART1_CLK          72000000
/* APB1时钟频率 (USART2/3) */
#define USART2_CLK          36000000
#define USART3_CLK          36000000

/**
 * @brief 计算波特率寄存器值
 * @param clk 时钟频率
 * @param baud 目标波特率
 * @return BRR值
 */
static uint16_t USART_ComputeBRR(uint32_t clk, uint32_t baud)
{
    uint32_t integerdivider;
    uint32_t fractionaldivider;

    /* 计算整数部分 */
    integerdivider = ((25 * clk) / (4 * baud));

    /* 计算BRR值 */
    uint16_t brr = (uint16_t)((integerdivider / 100) << 4);

    /* 计算小数部分 */
    fractionaldivider = integerdivider - (100 * (brr >> 4));

    /* 四舍五入 */
    brr |= ((((fractionaldivider * 16) + 50) / 100)) & 0x0F;

    return brr;
}

/* ============================================================================
 * 轮询模式API
 * ============================================================================ */

/**
 * @brief 初始化USART (轮询模式)
 * @param USARTx USART指针
 * @param BaudRate 波特率
 * @details 使用示例:
 *          USART_Init_Polling(USART1, 115200);  // 配置USART1为115200波特率
 */
void USART_Init_Polling(USART_TypeDef *USARTx, uint32_t BaudRate)
{
    uint32_t clk;
    uint16_t brr;

    /* 确定时钟频率 */
    if (USARTx == USART1) {
        clk = USART1_CLK;
        /* 使能USART1时钟和GPIO时钟 */
        (*(volatile uint32_t *)0x40021018) |= (1 << 14);  /* USART1EN */
        (*(volatile uint32_t *)0x40021018) |= (1 << 2);   /* IOPAEN */
    } else if (USARTx == USART2) {
        clk = USART2_CLK;
        /* 使能USART2时钟 */
        (*(volatile uint32_t *)0x4002101C) |= (1 << 17);  /* USART2EN */
        (*(volatile uint32_t *)0x40021018) |= (1 << 2);   /* IOPAEN */
    } else {
        clk = USART3_CLK;
        /* 使能USART3时钟 */
        (*(volatile uint32_t *)0x4002101C) |= (1 << 18);  /* USART3EN */
        (*(volatile uint32_t *)0x40021018) |= (1 << 3);   /* IOPBEN */
    }

    /* 计算波特率 */
    brr = USART_ComputeBRR(clk, BaudRate);
    USARTx->BRR = brr;

    /* 配置: 8数据位, 无校验, 1停止位, 使能发送和接收 */
    USARTx->CR1 = (1 << USART_CR1_TE_Pos) |
                  (1 << USART_CR1_RE_Pos) |
                  (1 << USART_CR1_UE_Pos);

    /* 1个停止位 */
    USARTx->CR2 = 0;

    /* 禁用硬件流控 */
    USARTx->CR3 = 0;
}

/**
 * @brief 发送单个字符(轮询模式)
 * @param USARTx USART指针
 * @param ch 要发送的字符
 */
void USART_SendChar(USART_TypeDef *USARTx, char ch)
{
    /* 等待发送数据寄存器为空 */
    while (!(USARTx->SR & (1 << USART_SR_TXE_Pos)));

    /* 发送数据 */
    USARTx->DR = ch;
}

/**
 * @brief 接收单个字符(轮询模式，阻塞)
 * @param USARTx USART指针
 * @return 接收到的字符
 */
char USART_ReceiveChar(USART_TypeDef *USARTx)
{
    /* 等待接收数据就绪 */
    while (!(USARTx->SR & (1 << USART_SR_RXNE_Pos)));

    /* 返回数据 */
    return (char)(USARTx->DR & 0xFF);
}

/**
 * @brief 检查是否有数据可接收
 * @param USARTx USART指针
 * @return 1=有数据, 0=无数据
 */
static inline uint8_t USART_DataAvailable(USART_TypeDef *USARTx)
{
    return (USARTx->SR & (1 << USART_SR_RXNE_Pos)) ? 1 : 0;
}

/**
 * @brief 非阻塞接收字符
 * @param USARTx USART指针
 * @param ch 接收缓冲区指针
 * @return 1=成功接收, 0=无数据
 */
uint8_t USART_ReceiveChar_NonBlocking(USART_TypeDef *USARTx, char *ch)
{
    if (USART_DataAvailable(USARTx)) {
        *ch = (char)(USARTx->DR & 0xFF);
        return 1;
    }
    return 0;
}

/**
 * @brief 发送字符串
 * @param USARTx USART指针
 * @param str 字符串指针
 */
void USART_SendString(USART_TypeDef *USARTx, const char *str)
{
    while (*str) {
        USART_SendChar(USARTx, *str++);
    }
}

/**
 * @brief 发送缓冲区数据
 * @param USARTx USART指针
 * @param data 数据缓冲区
 * @param len 数据长度
 */
void USART_SendBuffer(USART_TypeDef *USARTx, const uint8_t *data, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++) {
        USART_SendChar(USARTx, (char)data[i]);
    }
}

/**
 * @brief 等待发送完成
 * @param USARTx USART指针
 */
void USART_WaitForTXComplete(USART_TypeDef *USARTx)
{
    while (!(USARTx->SR & (1 << USART_SR_TC_Pos)));
}

/**
 * @brief 格式化打印(简化版printf)
 * @param USARTx USART指针
 * @param format 格式字符串
 * @details 支持: %d(整数), %u(无符号), %x(十六进制), %s(字符串), %c(字符)
 */
void USART_Printf(USART_TypeDef *USARTx, const char *format, ...)
{
    /* 这里简化实现，实际需要处理va_list */
    /* 完整实现请参考项目模板中的printf函数 */
    USART_SendString(USARTx, format);
}

/* ============================================================================
 * 中断模式API
 * ============================================================================ */

/* 接收缓冲区大小 */
#define USART_RX_BUF_SIZE   128
#define USART_TX_BUF_SIZE   128

/* USART1 缓冲区 */
static volatile uint8_t  g_usart1_rx_buf[USART_RX_BUF_SIZE];
static volatile uint16_t g_usart1_rx_head = 0;
static volatile uint16_t g_usart1_rx_tail = 0;

static volatile uint8_t  g_usart1_tx_buf[USART_TX_BUF_SIZE];
static volatile uint16_t g_usart1_tx_head = 0;
static volatile uint16_t g_usart1_tx_tail = 0;
static volatile uint8_t  g_usart1_tx_busy = 0;

/**
 * @brief 初始化USART (中断模式)
 * @param USARTx USART指针
 * @param BaudRate 波特率
 * @details 使用中断进行接收，接收缓冲区满时丢弃新数据
 */
void USART_Init_Interrupt(USART_TypeDef *USARTx, uint32_t BaudRate)
{
    /* 先进行轮询模式的基本初始化 */
    USART_Init_Polling(USARTx, BaudRate);

    /* 清空缓冲区 */
    if (USARTx == USART1) {
        g_usart1_rx_head = 0;
        g_usart1_rx_tail = 0;
        g_usart1_tx_head = 0;
        g_usart1_tx_tail = 0;
        g_usart1_tx_busy = 0;

        /* 使能RXNE中断 */
        USARTx->CR1 |= (1 << USART_CR1_RXNEIE_Pos);

        /* NVIC配置: 使能USART1中断 */
        (*(volatile uint32_t *)0xE000E100) |= (1 << 37);  /* NVIC_ISER[37] = USART1 */
    }
    /* 其他USART类似... */
}

/**
 * @brief USART1中断处理函数
 * @note 需要在启动文件中定义USART1_IRQHandler
 */
void USART1_IRQHandler_User(void)
{
    uint32_t sr = USART1->SR;

    /* 接收中断 */
    if (sr & (1 << USART_SR_RXNE_Pos)) {
        uint8_t data = (uint8_t)(USART1->DR & 0xFF);

        uint16_t next_head = (g_usart1_rx_head + 1) % USART_RX_BUF_SIZE;
        if (next_head != g_usart1_rx_tail) {
            g_usart1_rx_buf[g_usart1_rx_head] = data;
            g_usart1_rx_head = next_head;
        }
        /* 缓冲区满时丢弃数据 */
    }

    /* 发送中断 */
    if (sr & (1 << USART_SR_TXE_Pos)) {
        if (g_usart1_tx_head != g_usart1_tx_tail) {
            /* 发送下一个字节 */
            USART1->DR = g_usart1_tx_buf[g_usart1_tx_tail];
            g_usart1_tx_tail = (g_usart1_tx_tail + 1) % USART_TX_BUF_SIZE;
        } else {
            /* 发送完成，禁用TXE中断 */
            USART1->CR1 &= ~(1 << USART_CR1_TXEIE_Pos);
            g_usart1_tx_busy = 0;
        }
    }
}

/**
 * @brief 中断模式发送字符
 * @param USARTx USART指针
 * @param ch 要发送的字符
 * @return 1=成功加入发送队列, 0=发送缓冲区满
 */
uint8_t USART_SendChar_IT(USART_TypeDef *USARTx, char ch)
{
    if (USARTx != USART1) return 0;

    uint16_t next_head = (g_usart1_tx_head + 1) % USART_TX_BUF_SIZE;
    if (next_head == g_usart1_tx_tail) {
        return 0;  /* 缓冲区满 */
    }

    g_usart1_tx_buf[g_usart1_tx_head] = (uint8_t)ch;
    g_usart1_tx_head = next_head;

    /* 如果发送器空闲，启动发送 */
    if (!g_usart1_tx_busy) {
        g_usart1_tx_busy = 1;
        USARTx->CR1 |= (1 << USART_CR1_TXEIE_Pos);
    }

    return 1;
}

/**
 * @brief 中断模式发送字符串
 * @param USARTx USART指针
 * @param str 字符串指针
 * @return 发送的字符数
 */
uint16_t USART_SendString_IT(USART_TypeDef *USARTx, const char *str)
{
    uint16_t count = 0;
    while (*str) {
        if (USART_SendChar_IT(USARTx, *str++)) {
            count++;
        } else {
            break;  /* 缓冲区满 */
        }
    }
    return count;
}

/**
 * @brief 获取接收缓冲区中可用的字节数
 * @param USARTx USART指针
 * @return 可读取的字节数
 */
uint16_t USART_GetRxCount(USART_TypeDef *USARTx)
{
    if (USARTx != USART1) return 0;

    return (g_usart1_rx_head - g_usart1_rx_tail + USART_RX_BUF_SIZE) % USART_RX_BUF_SIZE;
}

/**
 * @brief 从接收缓冲区读取一个字节
 * @param USARTx USART指针
 * @param data 数据缓冲区
 * @return 1=成功, 0=无数据
 */
uint8_t USART_ReadRxBuffer(USART_TypeDef *USARTx, uint8_t *data)
{
    if (USARTx != USART1) return 0;

    if (g_usart1_rx_head == g_usart1_rx_tail) {
        return 0;  /* 缓冲区空 */
    }

    *data = g_usart1_rx_buf[g_usart1_rx_tail];
    g_usart1_rx_tail = (g_usart1_rx_tail + 1) % USART_RX_BUF_SIZE;

    return 1;
}

/**
 * @brief 读取一行数据(以\r或\n结尾)
 * @param USARTx USART指针
 * @param buffer 接收缓冲区
 * @param max_len 最大长度
 * @return 读取的字节数
 */
uint16_t USART_ReadLine(USART_TypeDef *USARTx, char *buffer, uint16_t max_len)
{
    uint16_t count = 0;
    uint8_t data;

    while (count < max_len - 1) {
        if (USART_ReadRxBuffer(USARTx, &data)) {
            buffer[count++] = (char)data;

            if (data == '\n' || data == '\r') {
                break;
            }
        } else {
            break;  /* 无更多数据 */
        }
    }

    buffer[count] = '\0';
    return count;
}

/**
 * @brief 清空接收缓冲区
 * @param USARTx USART指针
 */
void USART_FlushRxBuffer(USART_TypeDef *USARTx)
{
    if (USARTx == USART1) {
        g_usart1_rx_head = 0;
        g_usart1_rx_tail = 0;
    }
}
