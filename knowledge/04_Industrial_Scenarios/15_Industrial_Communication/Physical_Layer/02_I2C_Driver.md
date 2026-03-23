---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
﻿# I2C (Inter-Integrated Circuit) 驱动开发指南

> 两线串行总线协议及驱动实现

---

## I2C 协议概述

I2C是由Philips开发的半双工同步串行总线，使用两根信号线：

- **SDA** (Serial Data): 数据线
- **SCL** (Serial Clock): 时钟线

| 特性 | 规格 |
|:-----|:-----|
| 信号线 | 2根 (SDA + SCL) |
| 设备数量 | 最多127个 (7位地址) |
| 速度模式 | 标准100kbps, 快速400kbps, 高速3.4Mbps |
| 拓扑 | 多主多从 |
| 电气 | 开漏输出，需上拉电阻 |

---


---

## 📑 目录

- [I2C (Inter-Integrated Circuit) 驱动开发指南](#i2c-inter-integrated-circuit-驱动开发指南)
  - [I2C 协议概述](#i2c-协议概述)
  - [📑 目录](#-目录)
  - [I2C 时序分析](#i2c-时序分析)
    - [时序参数](#时序参数)
  - [I2C 硬件驱动](#i2c-硬件驱动)
    - [寄存器定义](#寄存器定义)
    - [初始化配置](#初始化配置)
    - [主机发送](#主机发送)
    - [主机接收](#主机接收)
  - [软件I2C (Bit-Bang)](#软件i2c-bit-bang)
  - [设备驱动示例](#设备驱动示例)
    - [EEPROM AT24Cxx驱动](#eeprom-at24cxx驱动)
    - [温度传感器 TMP102驱动](#温度传感器-tmp102驱动)
  - [调试技巧](#调试技巧)
    - [常见问题排查](#常见问题排查)
    - [逻辑分析仪捕获](#逻辑分析仪捕获)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## I2C 时序分析

```
        SCL  ─┐   ┌───┐   ┌───┐   ┌───┐   ┌───┐      ┌───┐
              └───┘   └───┘   └───┘   └───┘   └──────┘
        SDA  ───┐   ┌─────┐     ┌───────┐         ┌────
                  └──┘     └───┘         └───────┘
              |Start|  D7  | D6 |  ...  | D0 | ACK |Stop|
```

### 时序参数

| 参数 | 标准模式 | 快速模式 | 符号 |
|:-----|:--------:|:--------:|:-----|
| SCL时钟频率 | 100kHz | 400kHz | fSCL |
| 起始保持时间 | 4.0μs | 0.6μs | tHD;STA |
| SCL低电平时间 | 4.7μs | 1.3μs | tLOW |
| SCL高电平时间 | 4.0μs | 0.6μs | tHIGH |
| 数据建立时间 | 250ns | 100ns | tSU;DAT |
| 数据保持时间 | 5.0μs | 0.9μs | tHD;DAT |

---

## I2C 硬件驱动

### 寄存器定义

```c
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
```

### 初始化配置

```c
typedef enum {
    I2C_SPEED_STANDARD = 100000,   /* 100kHz */
    I2C_SPEED_FAST = 400000        /* 400kHz */
} I2C_Speed;

typedef struct {
    I2C_Speed speed;
    uint32_t own_address;          /* 自身地址 (从模式) */
    uint8_t duty_cycle;            /* 快速模式占空比 */
} I2C_Config;

/**
 * I2C初始化
 */
void I2C_Init(I2C_TypeDef *I2Cx, const I2C_Config *config, uint32_t pclk1)
{
    /* 禁用I2C */
    I2Cx->CR1 &= ~I2C_CR1_PE;

    /* 配置时钟 */
    uint32_t freq_range = pclk1 / 1000000;  /* MHz */
    I2Cx->CR2 = freq_range;

    /* 配置CCR */
    uint32_t ccr;
    if (config->speed == I2C_SPEED_STANDARD) {
        /* 标准模式: CCR = Fpclk1 / (2 * Fi2c) */
        ccr = pclk1 / (2 * I2C_SPEED_STANDARD);
        if (ccr < 4) ccr = 4;
    } else {
        /* 快速模式 */
        if (config->duty_cycle == 0) {
            /* 占空比 2:1 */
            ccr = pclk1 / (3 * I2C_SPEED_FAST);
        } else {
            /* 占空比 16:9 */
            ccr = pclk1 / (25 * I2C_SPEED_FAST);
            ccr |= (1U << 15);  /* 设置占空比位 */
        }
        ccr |= (1U << 15);  /* 快速模式 */
    }
    I2Cx->CCR = ccr;

    /* 配置最大上升时间 */
    if (config->speed == I2C_SPEED_STANDARD) {
        I2Cx->TRISE = freq_range + 1;  /* 标准模式 */
    } else {
        I2Cx->TRISE = (freq_range * 300 / 1000) + 1;  /* 快速模式300ns */
    }

    /* 配置自身地址 (从模式) */
    if (config->own_address != 0) {
        I2Cx->OAR1 = (config->own_address << 1) | (1U << 14);  /* 7位地址 */
    }

    /* 使能I2C */
    I2Cx->CR1 |= I2C_CR1_PE;
}
```

### 主机发送

```c
/**
 * I2C主机发送数据
 *
 * @param I2Cx I2C实例
 * @param slave_addr 从机地址 (7位，左对齐)
 * @param data 数据缓冲区
 * @param size 数据长度
 * @return 0=成功, 其他=错误码
 */
int I2C_Master_Transmit(I2C_TypeDef *I2Cx, uint16_t slave_addr,
                        const uint8_t *data, uint16_t size, uint32_t timeout)
{
    uint32_t tickstart = get_tick();

    /* 等待总线空闲 */
    while (I2Cx->SR2 & (1U << 1)) {
        if ((get_tick() - tickstart) > timeout) {
            return I2C_ERROR_BUSY;
        }
    }

    /* 发送起始条件 */
    I2Cx->CR1 |= I2C_CR1_START;

    /* 等待起始条件发送完成 */
    while (!(I2Cx->SR1 & I2C_SR1_SB)) {
        if ((get_tick() - tickstart) > timeout) {
            return I2C_ERROR_TIMEOUT;
        }
    }

    /* 发送从机地址 (写模式) */
    I2Cx->DR = (slave_addr << 1) & 0xFE;

    /* 等待地址发送完成 */
    while (!(I2Cx->SR1 & I2C_SR1_ADDR)) {
        if ((get_tick() - tickstart) > timeout) {
            return I2C_ERROR_ADDR;
        }
    }

    /* 清除ADDR标志 */
    (void)I2Cx->SR1;
    (void)I2Cx->SR2;

    /* 发送数据 */
    for (uint16_t i = 0; i < size; i++) {
        /* 等待发送寄存器空 */
        while (!(I2Cx->SR1 & I2C_SR1_TXE)) {
            if ((get_tick() - tickstart) > timeout) {
                return I2C_ERROR_TIMEOUT;
            }
        }

        I2Cx->DR = data[i];
    }

    /* 等待字节发送完成 */
    while (!(I2Cx->SR1 & I2C_SR1_BTF)) {
        if ((get_tick() - tickstart) > timeout) {
            return I2C_ERROR_TIMEOUT;
        }
    }

    /* 发送停止条件 */
    I2Cx->CR1 |= I2C_CR1_STOP;

    return I2C_OK;
}
```

### 主机接收

```c
/**
 * I2C主机接收数据
 */
int I2C_Master_Receive(I2C_TypeDef *I2Cx, uint16_t slave_addr,
                       uint8_t *data, uint16_t size, uint32_t timeout)
{
    uint32_t tickstart = get_tick();

    /* 使能ACK */
    I2Cx->CR1 |= I2C_CR1_ACK;

    /* 发送起始条件 */
    I2Cx->CR1 |= I2C_CR1_START;
    while (!(I2Cx->SR1 & I2C_SR1_SB));

    /* 发送从机地址 (读模式) */
    I2Cx->DR = (slave_addr << 1) | 0x01;
    while (!(I2Cx->SR1 & I2C_SR1_ADDR));

    if (size == 1) {
        /* 单字节接收 */
        I2Cx->CR1 &= ~I2C_CR1_ACK;  /* 禁用ACK */
        __disable_irq();  /* 临界区 */
        (void)I2Cx->SR1;
        (void)I2Cx->SR2;
        I2Cx->CR1 |= I2C_CR1_STOP;  /* 发送停止 */
        __enable_irq();

        while (!(I2Cx->SR1 & I2C_SR1_RXNE));
        *data = I2Cx->DR;
    } else if (size == 2) {
        /* 双字节接收 */
        I2Cx->CR1 |= I2C_CR1_POS;   /* 设置POS位 */
        __disable_irq();
        (void)I2Cx->SR1;
        (void)I2Cx->SR2;
        I2Cx->CR1 &= ~I2C_CR1_ACK;  /* 清除ACK */
        __enable_irq();

        while (!(I2Cx->SR1 & I2C_SR1_BTF));
        __disable_irq();
        I2Cx->CR1 |= I2C_CR1_STOP;
        *data++ = I2Cx->DR;
        __enable_irq();
        *data = I2Cx->DR;
    } else {
        /* 多字节接收 */
        (void)I2Cx->SR1;
        (void)I2Cx->SR2;

        for (uint16_t i = size; i > 0; i--) {
            if (i == 3) {
                /* 倒数第三字节 */
                while (!(I2Cx->SR1 & I2C_SR1_BTF));
                I2Cx->CR1 &= ~I2C_CR1_ACK;
                __disable_irq();
                *data++ = I2Cx->DR;
                I2Cx->CR1 |= I2C_CR1_STOP;
                __enable_irq();
            } else if (i == 2) {
                /* 倒数第二字节 */
                while (!(I2Cx->SR1 & I2C_SR1_BTF));
            }

            while (!(I2Cx->SR1 & I2C_SR1_RXNE));
            *data++ = I2Cx->DR;
        }
    }

    return I2C_OK;
}
```

---

## 软件I2C (Bit-Bang)

```c
/**
 * 软件模拟I2C实现
 * 适用于没有硬件I2C或引脚冲突的场景
 */

/* 引脚定义 */
#define SOFT_I2C_SDA_PORT   GPIOB
#define SOFT_I2C_SDA_PIN    GPIO_PIN_7
#define SOFT_I2C_SCL_PORT   GPIOB
#define SOFT_I2C_SCL_PIN    GPIO_PIN_6

/* 延时宏 - 约5us @ 72MHz */
#define I2C_DELAY()         { for(volatile int i=0; i<20; i++); }

/* 控制宏 */
#define SDA_HIGH()          GPIO_WritePin(SOFT_I2C_SDA_PORT, SOFT_I2C_SDA_PIN, 1)
#define SDA_LOW()           GPIO_WritePin(SOFT_I2C_SDA_PORT, SOFT_I2C_SDA_PIN, 0)
#define SDA_READ()          GPIO_ReadPin(SOFT_I2C_SDA_PORT, SOFT_I2C_SDA_PIN)
#define SCL_HIGH()          GPIO_WritePin(SOFT_I2C_SCL_PORT, SOFT_I2C_SCL_PIN, 1)
#define SCL_LOW()           GPIO_WritePin(SOFT_I2C_SCL_PORT, SOFT_I2C_SCL_PIN, 0)

void SoftI2C_Init(void)
{
    /* 配置为开漏输出 */
    GPIO_Config config = {
        .mode = GPIO_MODE_OUTPUT_OD,
        .speed = GPIO_SPEED_HIGH
    };

    config.pin = SOFT_I2C_SDA_PIN;
    GPIO_Init(SOFT_I2C_SDA_PORT, &config);

    config.pin = SOFT_I2C_SCL_PIN;
    GPIO_Init(SOFT_I2C_SCL_PORT, &config);

    /* 初始状态高 */
    SDA_HIGH();
    SCL_HIGH();
}

void SoftI2C_Start(void)
{
    SDA_HIGH();
    SCL_HIGH();
    I2C_DELAY();
    SDA_LOW();
    I2C_DELAY();
    SCL_LOW();
}

void SoftI2C_Stop(void)
{
    SDA_LOW();
    SCL_HIGH();
    I2C_DELAY();
    SDA_HIGH();
    I2C_DELAY();
}

void SoftI2C_SendByte(uint8_t byte)
{
    for (int i = 7; i >= 0; i--) {
        SCL_LOW();
        if (byte & (1 << i)) {
            SDA_HIGH();
        } else {
            SDA_LOW();
        }
        I2C_DELAY();
        SCL_HIGH();
        I2C_DELAY();
    }
    SCL_LOW();
}

uint8_t SoftI2C_ReadByte(uint8_t ack)
{
    uint8_t byte = 0;

    SDA_HIGH();  /* 释放SDA */

    for (int i = 7; i >= 0; i--) {
        SCL_LOW();
        I2C_DELAY();
        SCL_HIGH();
        I2C_DELAY();
        if (SDA_READ()) {
            byte |= (1 << i);
        }
    }
    SCL_LOW();

    /* 发送ACK/NACK */
    if (ack) {
        SDA_LOW();
    } else {
        SDA_HIGH();
    }
    I2C_DELAY();
    SCL_HIGH();
    I2C_DELAY();
    SCL_LOW();

    return byte;
}

uint8_t SoftI2C_WaitAck(void)
{
    uint8_t ack;

    SDA_HIGH();
    I2C_DELAY();
    SCL_HIGH();
    I2C_DELAY();
    ack = SDA_READ();
    SCL_LOW();

    return ack;  /* 0=ACK, 1=NACK */
}
```

---

## 设备驱动示例

### EEPROM AT24Cxx驱动

```c
/**
 * AT24Cxx EEPROM驱动
 * 容量: 1Kbit - 512Kbit
 */

#define AT24C_ADDR      0x50    /* 7位地址 */
#define AT24C_PAGE_SIZE 32      /* 页大小 */

/**
 * 写一个字节
 */
int AT24C_WriteByte(I2C_TypeDef *I2Cx, uint16_t mem_addr, uint8_t data)
{
    uint8_t tx_data[2];
    tx_data[0] = (mem_addr >> 8) & 0xFF;  /* 高地址 */
    tx_data[1] = mem_addr & 0xFF;          /* 低地址 */

    /* 发送地址 */
    int ret = I2C_Master_Transmit(I2Cx, AT24C_ADDR, tx_data, 2, 100);
    if (ret != I2C_OK) return ret;

    /* 发送数据 */
    ret = I2C_Master_Transmit(I2Cx, AT24C_ADDR, &data, 1, 100);

    /* 等待写入完成 */
    delay_ms(5);

    return ret;
}

/**
 * 页写
 */
int AT24C_PageWrite(I2C_TypeDef *I2Cx, uint16_t mem_addr,
                    const uint8_t *data, uint8_t size)
{
    if (size > AT24C_PAGE_SIZE) {
        size = AT24C_PAGE_SIZE;
    }

    uint8_t tx_data[AT24C_PAGE_SIZE + 2];
    tx_data[0] = (mem_addr >> 8) & 0xFF;
    tx_data[1] = mem_addr & 0xFF;
    memcpy(&tx_data[2], data, size);

    int ret = I2C_Master_Transmit(I2Cx, AT24C_ADDR, tx_data, size + 2, 100);

    delay_ms(5);

    return ret;
}

/**
 * 顺序读
 */
int AT24C_Read(I2C_TypeDef *I2Cx, uint16_t mem_addr, uint8_t *data, uint16_t size)
{
    uint8_t addr_data[2];
    addr_data[0] = (mem_addr >> 8) & 0xFF;
    addr_data[1] = mem_addr & 0xFF;

    /* 发送读地址 */
    int ret = I2C_Master_Transmit(I2Cx, AT24C_ADDR, addr_data, 2, 100);
    if (ret != I2C_OK) return ret;

    /* 接收数据 */
    ret = I2C_Master_Receive(I2Cx, AT24C_ADDR, data, size, 100);

    return ret;
}
```

### 温度传感器 TMP102驱动

```c
/**
 * TMP102 数字温度传感器驱动
 * 精度: 0.0625°C
 * 接口: I2C
 */

#define TMP102_ADDR     0x48
#define TMP102_REG_TEMP 0x00
#define TMP102_REG_CONF 0x01

typedef struct {
    float temperature;
    bool valid;
} TMP102_Result;

/**
 * 读取温度
 */
TMP102_Result TMP102_ReadTemperature(I2C_TypeDef *I2Cx)
{
    TMP102_Result result = {0, false};
    uint8_t reg = TMP102_REG_TEMP;
    uint8_t data[2];

    /* 发送寄存器地址 */
    if (I2C_Master_Transmit(I2Cx, TMP102_ADDR, &reg, 1, 100) != I2C_OK) {
        return result;
    }

    /* 读取2字节数据 */
    if (I2C_Master_Receive(I2Cx, TMP102_ADDR, data, 2, 100) != I2C_OK) {
        return result;
    }

    /* 转换温度 */
    int16_t temp_raw = ((int16_t)data[0] << 4) | (data[1] >> 4);
    if (temp_raw & 0x800) {  /* 负数 */
        temp_raw |= 0xF000;
    }

    result.temperature = temp_raw * 0.0625f;
    result.valid = true;

    return result;
}
```

---

## 调试技巧

### 常见问题排查

| 问题 | 可能原因 | 解决方案 |
|:-----|:---------|:---------|
| 总线忙 | 上次传输未完成 | 发送STOP或复位I2C |
| NACK | 设备未响应 | 检查地址和连接 |
| 数据错误 | 时序问题 | 降低速度，检查上拉 |
| 仲裁丢失 | 多主机冲突 | 实现重试机制 |

### 逻辑分析仪捕获

```c
/* 添加调试输出 */
#ifdef I2C_DEBUG
    #define I2C_LOG(fmt, ...) printf("[I2C] " fmt "\n", ##__VA_ARGS__)
#else
    #define I2C_LOG(fmt, ...)
#endif

/* 在关键位置添加日志 */
int I2C_Master_Transmit(...)
{
    I2C_LOG("Start transmit to 0x%02X, %d bytes", slave_addr, size);
    /* ... */
    if (ret != I2C_OK) {
        I2C_LOG("Error: %d", ret);
    }
    return ret;
}
```

---

**参考**: NXP I2C-bus specification and user manual, UM10204


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
