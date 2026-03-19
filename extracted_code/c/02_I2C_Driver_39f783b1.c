/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\Physical_Layer\02_I2C_Driver.md
 * Line: 77
 * Language: c
 * Block ID: 39f783b1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * I2C寄存器结构 (ARM Cortex-M典型实现)
 */
typedef struct {
    volatile uint32_t CR1;         /* 控制寄存器1 */
    volatile uint32_t CR2;         /* 控制寄存器2 */
    volatile uint32_t OAR1;        /* 自身地址寄存器1 */
    volatile uint32_t OAR2;        /* 自身地址寄存器2 */
    volatile uint32_t DR;          /* 数据寄存器 */
    volatile uint32_t SR1;         /* 状态寄存器1 */
    volatile uint32_t SR2;         /* 状态寄存器2 */
    volatile uint32_t CCR;         /* 时钟控制寄存器 */
    volatile uint32_t TRISE;       /* 上升时间寄存器 */
    volatile uint32_t FLTR;        /* 滤波寄存器 */
} I2C_TypeDef;

#define I2C1_BASE       0x40005400
#define I2C2_BASE       0x40005800
#define I2C3_BASE       0x40005C00

#define I2C1            ((I2C_TypeDef *)I2C1_BASE)
#define I2C2            ((I2C_TypeDef *)I2C2_BASE)
#define I2C3            ((I2C_TypeDef *)I2C3_BASE)

/* CR1位定义 */
#define I2C_CR1_PE      (1U << 0)   /* 外设使能 */
#define I2C_CR1_START   (1U << 8)   /* 起始条件 */
#define I2C_CR1_STOP    (1U << 9)   /* 停止条件 */
#define I2C_CR1_ACK     (1U << 10)  /* 应答使能 */

/* SR1位定义 */
#define I2C_SR1_SB      (1U << 0)   /* 起始位已发送 */
#define I2C_SR1_ADDR    (1U << 1)   /* 地址已发送 */
#define I2C_SR1_BTF     (1U << 2)   /* 字节发送完成 */
#define I2C_SR1_RXNE    (1U << 6)   /* 接收非空 */
#define I2C_SR1_TXE     (1U << 7)   /* 发送空 */
