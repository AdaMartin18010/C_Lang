# GPIO (General Purpose Input/Output) 驱动开发指南

> 通用输入输出接口的硬件驱动和软件控制

---

## GPIO 基础概念

GPIO是最基本的硬件接口，用于数字信号的输入输出控制。

| 特性 | 说明 |
|:-----|:-----|
| 电压电平 | 3.3V / 5V TTL/CMOS |
| 驱动能力 | 通常 4-16mA |
| 速度 | 可达几十MHz |
| 模式 | 输入/输出/复用/模拟 |

---


---

## 📑 目录

- [GPIO (General Purpose Input/Output) 驱动开发指南](#gpio-general-purpose-inputoutput-驱动开发指南)
  - [GPIO 基础概念](#gpio-基础概念)
  - [📑 目录](#-目录)
  - [GPIO 模式配置](#gpio-模式配置)
  - [GPIO 寄存器级操作](#gpio-寄存器级操作)
    - [寄存器定义 (基于内存映射)](#寄存器定义-基于内存映射)
    - [底层驱动实现](#底层驱动实现)
  - [位带操作 (Bit-Banding)](#位带操作-bit-banding)
  - [中断和事件](#中断和事件)
  - [应用示例](#应用示例)
    - [LED闪烁](#led闪烁)
    - [按键检测 (去抖)](#按键检测-去抖)
    - [软件I2C (位 bang)](#软件i2c-位-bang)
  - [GPIO设计注意事项](#gpio设计注意事项)
    - [电气特性](#电气特性)
    - [软件设计原则](#软件设计原则)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## GPIO 模式配置

```c
/**
 * GPIO模式定义
 * 基于STM32风格，适用于大多数ARM MCU
 */

typedef enum {
    GPIO_MODE_INPUT = 0,           /* 浮空输入 */
    GPIO_MODE_INPUT_PU,            /* 上拉输入 */
    GPIO_MODE_INPUT_PD,            /* 下拉输入 */
    GPIO_MODE_OUTPUT_PP,           /* 推挽输出 */
    GPIO_MODE_OUTPUT_OD,           /* 开漏输出 */
    GPIO_MODE_AF_PP,               /* 复用推挽 */
    GPIO_MODE_AF_OD,               /* 复用开漏 */
    GPIO_MODE_ANALOG               /* 模拟模式 */
} GPIO_Mode;

typedef enum {
    GPIO_SPEED_LOW = 0,            /* 2MHz */
    GPIO_SPEED_MEDIUM,             /* 10MHz */
    GPIO_SPEED_HIGH,               /* 50MHz */
    GPIO_SPEED_VERY_HIGH           /* 100MHz+ */
} GPIO_Speed;

typedef struct {
    uint32_t pin;                  /* GPIO引脚号 */
    GPIO_Mode mode;                /* 工作模式 */
    GPIO_Speed speed;              /* 输出速度 */
} GPIO_Config;
```

---

## GPIO 寄存器级操作

### 寄存器定义 (基于内存映射)

```c
/* GPIO寄存器结构 - ARM Cortex-M典型实现 */
typedef struct {
    volatile uint32_t MODER;       /* 模式寄存器 */
    volatile uint32_t OTYPER;      /* 输出类型寄存器 */
    volatile uint32_t OSPEEDR;     /* 输出速度寄存器 */
    volatile uint32_t PUPDR;       /* 上拉/下拉寄存器 */
    volatile uint32_t IDR;         /* 输入数据寄存器 */
    volatile uint32_t ODR;         /* 输出数据寄存器 */
    volatile uint32_t BSRR;        /* 位设置/清除寄存器 */
    volatile uint32_t LCKR;        /* 锁定寄存器 */
    volatile uint32_t AFR[2];      /* 复用功能寄存器 */
} GPIO_TypeDef;

/* GPIOA基地址 - 示例 */
#define GPIOA_BASE      0x40020000
#define GPIOB_BASE      0x40020400
#define GPIOC_BASE      0x40020800

#define GPIOA           ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB           ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC           ((GPIO_TypeDef *)GPIOC_BASE)

/* 引脚位定义 */
#define GPIO_PIN_0      (1U << 0)
#define GPIO_PIN_1      (1U << 1)
/* ... */
#define GPIO_PIN_15     (1U << 15)
```

### 底层驱动实现

```c
/**
 * GPIO初始化
 */
void GPIO_Init(GPIO_TypeDef *GPIOx, const GPIO_Config *config)
{
    uint32_t pin = config->pin;
    uint32_t pin_number = 0;

    /* 计算引脚号 */
    while ((pin >> pin_number) != 1) {
        pin_number++;
    }

    /* 配置模式 */
    uint32_t mode = (uint32_t)config->mode;
    GPIOx->MODER &= ~(3U << (pin_number * 2));  /* 清除 */
    GPIOx->MODER |= (mode & 3U) << (pin_number * 2);  /* 设置 */

    /* 配置输出类型 */
    if (mode == GPIO_MODE_OUTPUT_OD || mode == GPIO_MODE_AF_OD) {
        GPIOx->OTYPER |= pin;   /* 开漏 */
    } else {
        GPIOx->OTYPER &= ~pin;  /* 推挽 */
    }

    /* 配置速度 */
    GPIOx->OSPEEDR &= ~(3U << (pin_number * 2));
    GPIOx->OSPEEDR |= ((uint32_t)config->speed << (pin_number * 2));

    /* 配置上拉/下拉 */
    uint32_t pupd = 0;
    if (mode == GPIO_MODE_INPUT_PU) pupd = 1;
    else if (mode == GPIO_MODE_INPUT_PD) pupd = 2;

    GPIOx->PUPDR &= ~(3U << (pin_number * 2));
    GPIOx->PUPDR |= (pupd << (pin_number * 2));
}

/**
 * 设置引脚输出
 */
void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint32_t pin, uint8_t value)
{
    if (value) {
        GPIOx->BSRR = pin;           /* 设置 */
    } else {
        GPIOx->BSRR = pin << 16;     /* 清除 */
    }
}

/**
 * 读取引脚输入
 */
uint8_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint32_t pin)
{
    return (GPIOx->IDR & pin) ? 1 : 0;
}

/**
 * 翻转引脚输出
 */
void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint32_t pin)
{
    GPIOx->ODR ^= pin;
}
```

---

## 位带操作 (Bit-Banding)

```c
/**
 * 位带操作 - 原子性读写单个位
 * 适用于Cortex-M3/M4/M7
 */

/* 位带别名区计算 */
#define BITBAND_ADDR(addr, bitnum) (((addr) & 0xF0000000) + 0x2000000 + \
                                    (((addr) & 0xFFFFF) << 5) + ((bitnum) << 2))

#define MEM_ADDR(addr) *((volatile uint32_t *)(addr))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND_ADDR(addr, bitnum))

/* GPIO ODR位带 */
#define PAout(n)    BIT_ADDR(GPIOA_BASE + 0x14, n)   /* PAn输出 */
#define PAin(n)     BIT_ADDR(GPIOA_BASE + 0x10, n)   /* PAn输入 */

/* 使用示例 */
void bitband_example(void)
{
    /* 原子设置PA0 */
    PAout(0) = 1;

    /* 原子清除PA0 */
    PAout(0) = 0;

    /* 读取PA1 */
    uint8_t val = PAin(1);
}
```

---

## 中断和事件

```c
/**
 * GPIO外部中断配置
 */

/* 中断触发方式 */
typedef enum {
    GPIO_IT_RISING = 0,            /* 上升沿 */
    GPIO_IT_FALLING,               /* 下降沿 */
    GPIO_IT_RISING_FALLING         /* 双边沿 */
} GPIO_IT_Trigger;

/* 中断配置 */
typedef struct {
    uint32_t pin;
    GPIO_IT_Trigger trigger;
    void (*callback)(void);        /* 中断回调 */
} GPIO_IT_Config;

/* 外部中断配置 (简化示例) */
void GPIO_IT_Init(const GPIO_IT_Config *config)
{
    uint32_t pin = config->pin;
    uint32_t exti_line = 0;

    /* 计算EXTI线 */
    while ((pin >> exti_line) != 1) {
        exti_line++;
    }

    /* 配置EXTI */
    EXTI->IMR |= (1U << exti_line);   /* 使能中断 */

    /* 配置触发方式 */
    switch (config->trigger) {
        case GPIO_IT_RISING:
            EXTI->RTSR |= (1U << exti_line);
            EXTI->FTSR &= ~(1U << exti_line);
            break;
        case GPIO_IT_FALLING:
            EXTI->RTSR &= ~(1U << exti_line);
            EXTI->FTSR |= (1U << exti_line);
            break;
        case GPIO_IT_RISING_FALLING:
            EXTI->RTSR |= (1U << exti_line);
            EXTI->FTSR |= (1U << exti_line);
            break;
    }

    /* NVIC配置 */
    IRQn_Type irqn = (exti_line < 5) ? (EXTI0_IRQn + exti_line) :
                     (exti_line < 10) ? EXTI9_5_IRQn : EXTI15_10_IRQn;

    NVIC_EnableIRQ(irqn);
    NVIC_SetPriority(irqn, 5);
}

/* 中断处理示例 */
void EXTI0_IRQHandler(void)
{
    if (EXTI->PR & EXTI_PR_PR0) {
        EXTI->PR = EXTI_PR_PR0;  /* 清除中断标志 */

        /* 用户处理 */
        button_callback();
    }
}
```

---

## 应用示例

### LED闪烁

```c
/**
 * LED闪烁示例
 */

#define LED_PIN     GPIO_PIN_5
#define LED_GPIO    GPIOA

void LED_Init(void)
{
    GPIO_Config config = {
        .pin = LED_PIN,
        .mode = GPIO_MODE_OUTPUT_PP,
        .speed = GPIO_SPEED_LOW
    };

    GPIO_Init(LED_GPIO, &config);
}

void LED_On(void)
{
    GPIO_WritePin(LED_GPIO, LED_PIN, 1);
}

void LED_Off(void)
{
    GPIO_WritePin(LED_GPIO, LED_PIN, 0);
}

void LED_Toggle(void)
{
    GPIO_TogglePin(LED_GPIO, LED_PIN);
}

/* 主循环 */
void main_loop(void)
{
    LED_Init();

    while (1) {
        LED_Toggle();
        delay_ms(500);  /* 500ms闪烁 */
    }
}
```

### 按键检测 (去抖)

```c
/**
 * 按键检测 - 软件消抖
 */

#define BUTTON_PIN      GPIO_PIN_0
#define BUTTON_GPIO     GPIOA
#define DEBOUNCE_MS     20

void Button_Init(void)
{
    GPIO_Config config = {
        .pin = BUTTON_PIN,
        .mode = GPIO_MODE_INPUT_PU,  /* 上拉输入 */
        .speed = GPIO_SPEED_LOW
    };

    GPIO_Init(BUTTON_GPIO, &config);
}

/* 读取按键状态 (带消抖) */
uint8_t Button_Read(void)
{
    static uint8_t last_state = 1;  /* 上拉默认为高 */
    static uint32_t last_time = 0;

    uint8_t current_state = GPIO_ReadPin(BUTTON_GPIO, BUTTON_PIN);
    uint32_t current_time = get_tick_ms();

    /* 状态变化检测 */
    if (current_state != last_state) {
        /* 消抖时间检查 */
        if ((current_time - last_time) > DEBOUNCE_MS) {
            last_state = current_state;
            last_time = current_time;

            /* 按下检测 (上拉输入，按下为低) */
            if (current_state == 0) {
                return 1;  /* 按下 */
            }
        }
    }

    return 0;  /* 无按键 */
}
```

### 软件I2C (位 bang)

```c
/**
 * 软件模拟I2C - GPIO实现
 */

#define I2C_SDA_PIN     GPIO_PIN_7
#define I2C_SCL_PIN     GPIO_PIN_6
#define I2C_GPIO        GPIOB

/* SDA控制宏 */
#define SDA_HIGH()      GPIO_WritePin(I2C_GPIO, I2C_SDA_PIN, 1)
#define SDA_LOW()       GPIO_WritePin(I2C_GPIO, I2C_SDA_PIN, 0)
#define SDA_READ()      GPIO_ReadPin(I2C_GPIO, I2C_SDA_PIN)

/* SCL控制宏 */
#define SCL_HIGH()      GPIO_WritePin(I2C_GPIO, I2C_SCL_PIN, 1)
#define SCL_LOW()       GPIO_WritePin(I2C_GPIO, I2C_SCL_PIN, 0)

void SoftI2C_Init(void)
{
    /* SCL - 开漏输出 */
    GPIO_Config scl_config = {
        .pin = I2C_SCL_PIN,
        .mode = GPIO_MODE_OUTPUT_OD,
        .speed = GPIO_SPEED_HIGH
    };
    GPIO_Init(I2C_GPIO, &scl_config);

    /* SDA - 开漏输出 */
    GPIO_Config sda_config = {
        .pin = I2C_SDA_PIN,
        .mode = GPIO_MODE_OUTPUT_OD,
        .speed = GPIO_SPEED_HIGH
    };
    GPIO_Init(I2C_GPIO, &sda_config);

    SDA_HIGH();
    SCL_HIGH();
}

/* I2C时序 */
void I2C_Delay(void)
{
    for (volatile int i = 0; i < 10; i++);  /* 约5us @ 72MHz */
}

void I2C_Start(void)
{
    SDA_HIGH();
    SCL_HIGH();
    I2C_Delay();
    SDA_LOW();
    I2C_Delay();
    SCL_LOW();
}

void I2C_Stop(void)
{
    SDA_LOW();
    SCL_HIGH();
    I2C_Delay();
    SDA_HIGH();
    I2C_Delay();
}

/* 发送一个字节 */
uint8_t I2C_SendByte(uint8_t byte)
{
    for (int i = 7; i >= 0; i--) {
        if (byte & (1 << i)) {
            SDA_HIGH();
        } else {
            SDA_LOW();
        }
        I2C_Delay();
        SCL_HIGH();
        I2C_Delay();
        SCL_LOW();
    }

    /* 等待ACK */
    SDA_HIGH();  /* 释放SDA */
    I2C_Delay();
    SCL_HIGH();
    I2C_Delay();
    uint8_t ack = SDA_READ();
    SCL_LOW();

    return ack;  /* 0=ACK, 1=NACK */
}
```

---

## GPIO设计注意事项

### 电气特性

| 参数 | 典型值 | 注意事项 |
|:-----|:-------|:---------|
| 输出电流 | 8-16mA | 不要超驱动能力 |
| 输入阻抗 | 高阻 | 悬空引脚需配置上拉/下拉 |
| 翻转速度 |  ns级 | 考虑EMI和信号完整性 |
| ESD防护 | 2kV | 外部接口需额外保护 |

### 软件设计原则

```c
/* 1. 原子操作 */
/* 使用位带或禁用中断 */
void atomic_gpio_op(void)
{
    __disable_irq();
    GPIO_WritePin(GPIOA, PIN, value);  /* 原子写 */
    __enable_irq();
}

/* 2. 输入滤波 */
/* 对机械开关等噪声源进行软件滤波 */

/* 3. 状态机设计 */
/* 复杂GPIO交互使用状态机 */
typedef enum {
    GPIO_STATE_IDLE,
    GPIO_STATE_START,
    GPIO_STATE_ACTIVE,
    GPIO_STATE_DONE
} GPIO_State;

/* 4. 错误处理 */
/* 检查GPIO配置有效性 */
```

---

**参考**: ARM Cortex-M4 Technical Reference Manual, STM32 Reference Manual


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
