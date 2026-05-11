# UART接口知识梳理

> **全称**: Universal Asynchronous Receiver/Transmitter
> **类型**: 异步串行通信
> **最后更新**: 2026-03-29

---

## 协议基础

### UART通信原理

```
        ┌─────────┐                    ┌─────────┐
        │  TXD    │────────────────────│  RXD    │
        │  Device │                    │  Device │
        │   A     │                    │   B     │
        │  RXD    │◀───────────────────│  TXD    │
        └─────────┘                    └─────────┘

        异步通信：无时钟线，依赖双方波特率一致
```

### 帧格式

```
Start │ D0 │ D1 │ D2 │ D3 │ D4 │ D5 │ D6 │ D7 │ Parity │ Stop
  0     b0   b1   b2   b3   b4   b5   b6   b7     P       1

┌─────┬──────────────────────────────┬────────┬──────┐
│Start│        数据位 (5-9位)         │ 校验位 │ 停止位│
│ 0   │    D0 (LSB) ~ D7/D8 (MSB)    │可选    │ 1/1.5/2位│
└─────┴──────────────────────────────┴────────┴──────┘
```

### 参数配置

| 参数 | 选项 | 说明 |
|------|------|------|
| **波特率** | 9600, 115200, 921600... | 双方必须一致 |
| **数据位** | 8 (常用), 9 | 有效数据位数 |
| **校验位** | None, Even, Odd | 错误检测 |
| **停止位** | 1, 1.5, 2 | 帧结束标识 |

---

## C语言驱动设计

### 数据结构与API设计

```c
/* UART配置结构体 */
typedef struct {
    uint32_t baudrate;      /* 波特率 */
    uint8_t  databits;      /* 数据位: 8, 9 */
    uint8_t  parity;        /* 校验: 0=无, 1=奇, 2=偶 */
    uint8_t  stopbits;      /* 停止位: 1, 2 */
    uint8_t  flow_control;  /* 流控: 0=无, 1=RTS/CTS */
} UART_Config;

/* UART句柄 */
typedef struct {
    UART_Registers *reg;    /* 寄存器基地址 */
    RingBuffer rx_buf;      /* 接收环形缓冲区 */
    RingBuffer tx_buf;      /* 发送环形缓冲区 */
    void (*tx_complete_cb)(void);  /* 发送完成回调 */
    void (*rx_complete_cb)(uint8_t *data, uint16_t len); /* 接收完成回调 */
} UART_Handle;

/* API设计 */
void UART_Init(UART_Handle *huart, UART_Config *config);
void UART_DeInit(UART_Handle *huart);
void UART_Transmit(UART_Handle *huart, uint8_t *data, uint16_t len);
void UART_Transmit_IT(UART_Handle *huart, uint8_t *data, uint16_t len);
void UART_Receive_IT(UART_Handle *huart, uint16_t len);
uint16_t UART_Read(UART_Handle *huart, uint8_t *buf, uint16_t max_len);
```

### 环形缓冲区实现

```c
/* 线程安全的环形缓冲区 */
typedef struct {
    volatile uint16_t head;     /* 写指针 */
    volatile uint16_t tail;     /* 读指针 */
    uint16_t mask;              /* 大小掩码 (大小必须是2的幂) */
    uint8_t *buffer;
} RingBuffer;

#define RB_SIZE 256  /* 必须是2的幂 */

/* 初始化 */
void RB_Init(RingBuffer *rb, uint8_t *buf, uint16_t size) {
    rb->head = 0;
    rb->tail = 0;
    rb->buffer = buf;
    rb->mask = size - 1;
}

/* 写入 - 中断上下文调用 */
bool RB_Write(RingBuffer *rb, uint8_t data) {
    uint16_t next = (rb->head + 1) & rb->mask;
    if (next == rb->tail) {
        return false;  /* 满 */
    }
    rb->buffer[rb->head] = data;
    rb->head = next;
    return true;
}

/* 读取 - 主循环调用 */
bool RB_Read(RingBuffer *rb, uint8_t *data) {
    if (rb->head == rb->tail) {
        return false;  /* 空 */
    }
    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) & rb->mask;
    return true;
}
```

---

## 高级特性

### RS485半双工控制

```c
/* RS485方向控制 */
typedef struct {
    UART_Handle *uart;
    GPIO_TypeDef *de_port;      /* Driver Enable */
    uint16_t de_pin;
    GPIO_TypeDef *re_port;      /* Receiver Enable */
    uint16_t re_pin;
} RS485_Handle;

/* 切换到发送模式 */
void RS485_SetTransmit(RS485_Handle *hrs485) {
    GPIO_Set(hrs485->re_port, hrs485->re_pin, 1);  /* 关闭接收 */
    GPIO_Set(hrs485->de_port, hrs485->de_pin, 1);  /* 使能发送 */
}

/* 切换到接收模式 - 需在最后一个字节发送完成后 */
void RS485_SetReceive(RS485_Handle *hrs485) {
    GPIO_Set(hrs485->de_port, hrs485->de_pin, 0);  /* 关闭发送 */
    GPIO_Set(hrs485->re_port, hrs485->re_pin, 0);  /* 使能接收 */
}
```

### DMA传输

```c
/* DMA发送 */
void UART_Transmit_DMA(UART_Handle *huart, uint8_t *data, uint16_t len) {
    /* 配置DMA源地址 */
    DMA_SetSource(huart->dma_tx, data);
    /* 配置DMA目标地址（UART数据寄存器） */
    DMA_SetDestination(huart->dma_tx, &huart->reg->DR);
    /* 配置传输长度 */
    DMA_SetLength(huart->dma_tx, len);
    /* 启动DMA */
    DMA_Start(huart->dma_tx);
    /* 使能UART DMA发送 */
    huart->reg->CR3 |= USART_CR3_DMAT;
}
```

---

## 文档列表

| 文档 | 内容 | 状态 |
|------|------|------|
| [01_UART_Fundamentals.md](./01_UART_Fundamentals.md) | UART基础理论 | ⏳ |
| [02_Baud_Rate_Generation.md](./02_Baud_Rate_Generation.md) | 波特率生成 | ⏳ |
| [03_Framing_and_Parity.md](./03_Framing_and_Parity.md) | 帧格式与校验 | ⏳ |
| [04_Flow_Control_RTS_CTS.md](./04_Flow_Control_RTS_CTS.md) | 流控机制 | ⏳ |
| [05_UART_Driver_Architecture.md](./05_UART_Driver_Architecture.md) | 驱动架构 | ⏳ |
| [06_Ring_Buffer_Implementation.md](./06_Ring_Buffer_Implementation.md) | 环形缓冲区 | ⏳ |
| [07_RS485_Half_Duplex.md](./07_RS485_Half_Duplex.md) | RS485半双工 | ⏳ |
| [08_Modbus_over_UART.md](./08_Modbus_over_UART.md) | Modbus RTU实现 | ⏳ |
