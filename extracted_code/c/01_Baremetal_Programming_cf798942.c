/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Baremetal\01_Baremetal_Programming.md
 * Line: 1551
 * Language: c
 * Block ID: cf798942
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file gpio_driver.c
 * @brief STM32F103 GPIO驱动程序
 * @details 提供完整的GPIO配置和操作功能
 */

#include <stdint.h>

/* ============================================================================
 * GPIO 寄存器定义
 * ============================================================================ */

typedef struct {
    volatile uint32_t CRL;      /* 端口配置低寄存器 (0x00) */
    volatile uint32_t CRH;      /* 端口配置高寄存器 (0x04) */
    volatile uint32_t IDR;      /* 端口输入数据寄存器 (0x08) */
    volatile uint32_t ODR;      /* 端口输出数据寄存器 (0x0C) */
    volatile uint32_t BSRR;     /* 端口位设置/清除寄存器 (0x10) */
    volatile uint32_t BRR;      /* 端口位清除寄存器 (0x14) */
    volatile uint32_t LCKR;     /* 端口配置锁定寄存器 (0x18) */
} GPIO_TypeDef;

/* GPIO 基地址 */
#define GPIOA_BASE          0x40010800
#define GPIOB_BASE          0x40010C00
#define GPIOC_BASE          0x40011000
#define GPIOD_BASE          0x40011400
#define GPIOE_BASE          0x40011800
#define GPIOF_BASE          0x40011C00
#define GPIOG_BASE          0x40012000

#define GPIOA               ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB               ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC               ((GPIO_TypeDef *)GPIOC_BASE)
#define GPIOD               ((GPIO_TypeDef *)GPIOD_BASE)
#define GPIOE               ((GPIO_TypeDef *)GPIOE_BASE)
#define GPIOF               ((GPIO_TypeDef *)GPIOF_BASE)
#define GPIOG               ((GPIO_TypeDef *)GPIOG_BASE)

/* ============================================================================
 * GPIO 模式定义
 * ============================================================================ */

typedef enum {
    GPIO_MODE_INPUT_ANALOG = 0x0,       /* 模拟输入 */
    GPIO_MODE_INPUT_FLOATING = 0x4,     /* 浮空输入 */
    GPIO_MODE_INPUT_PULL = 0x8,         /* 上拉/下拉输入 */

    GPIO_MODE_OUTPUT_PP_10MHz = 0x1,    /* 推挽输出, 10MHz */
    GPIO_MODE_OUTPUT_PP_2MHz = 0x2,     /* 推挽输出, 2MHz */
    GPIO_MODE_OUTPUT_PP_50MHz = 0x3,    /* 推挽输出, 50MHz */

    GPIO_MODE_OUTPUT_OD_10MHz = 0x5,    /* 开漏输出, 10MHz */
    GPIO_MODE_OUTPUT_OD_2MHz = 0x6,     /* 开漏输出, 2MHz */
    GPIO_MODE_OUTPUT_OD_50MHz = 0x7,    /* 开漏输出, 50MHz */

    GPIO_MODE_AF_PP_10MHz = 0x9,        /* 复用推挽, 10MHz */
    GPIO_MODE_AF_PP_2MHz = 0xA,         /* 复用推挽, 2MHz */
    GPIO_MODE_AF_PP_50MHz = 0xB,        /* 复用推挽, 50MHz */

    GPIO_MODE_AF_OD_10MHz = 0xD,        /* 复用开漏, 10MHz */
    GPIO_MODE_AF_OD_2MHz = 0xE,         /* 复用开漏, 2MHz */
    GPIO_MODE_AF_OD_50MHz = 0xF,        /* 复用开漏, 50MHz */
} GPIO_ModeTypeDef;

typedef enum {
    GPIO_PIN_0 = 0,
    GPIO_PIN_1 = 1,
    GPIO_PIN_2 = 2,
    GPIO_PIN_3 = 3,
    GPIO_PIN_4 = 4,
    GPIO_PIN_5 = 5,
    GPIO_PIN_6 = 6,
    GPIO_PIN_7 = 7,
    GPIO_PIN_8 = 8,
    GPIO_PIN_9 = 9,
    GPIO_PIN_10 = 10,
    GPIO_PIN_11 = 11,
    GPIO_PIN_12 = 12,
    GPIO_PIN_13 = 13,
    GPIO_PIN_14 = 14,
    GPIO_PIN_15 = 15,
    GPIO_PIN_ALL = 16,
} GPIO_PinTypeDef;

typedef enum {
    GPIO_LEVEL_LOW = 0,
    GPIO_LEVEL_HIGH = 1,
} GPIO_LevelTypeDef;

/* ============================================================================
 * GPIO 配置结构体
 * ============================================================================ */

typedef struct {
    GPIO_TypeDef *GPIOx;        /* GPIO端口 */
    uint16_t Pin;               /* 引脚号 (位图格式: 1<<n) */
    GPIO_ModeTypeDef Mode;      /* 工作模式 */
} GPIO_ConfigTypeDef;

/* ============================================================================
 * 内部辅助函数
 * ============================================================================ */

/**
 * @brief 获取GPIO端口的时钟位位置
 * @param GPIOx GPIO端口指针
 * @return 时钟位位置 (2-8)
 */
static uint8_t GPIO_GetClockPos(GPIO_TypeDef *GPIOx)
{
    if (GPIOx == GPIOA) return 2;
    if (GPIOx == GPIOB) return 3;
    if (GPIOx == GPIOC) return 4;
    if (GPIOx == GPIOD) return 5;
    if (GPIOx == GPIOE) return 6;
    if (GPIOx == GPIOF) return 7;
    if (GPIOx == GPIOG) return 8;
    return 0;
}

/* ============================================================================
 * GPIO 初始化函数
 * ============================================================================ */

/**
 * @brief 初始化单个GPIO引脚
 * @param GPIOx GPIO端口 (GPIOA-GPIOG)
 * @param PinNum 引脚号 (0-15)
 * @param Mode 工作模式
 * @details 配置示例:
 *          GPIO_InitPin(GPIOA, 5, GPIO_MODE_OUTPUT_PP_50MHz);  // PA5推挽输出
 *          GPIO_InitPin(GPIOA, 0, GPIO_MODE_INPUT_PULL);        // PA0上拉输入
 */
void GPIO_InitPin(GPIO_TypeDef *GPIOx, uint8_t PinNum, GPIO_ModeTypeDef Mode)
{
    uint32_t config;

    if (PinNum < 8) {
        /* 低8位引脚: 使用CRL寄存器 */
        config = GPIOx->CRL;
        config &= ~(0xF << (PinNum * 4));           /* 清除原有配置 */
        config |= (Mode << (PinNum * 4));           /* 设置新模式 */
        GPIOx->CRL = config;
    } else {
        /* 高8位引脚: 使用CRH寄存器 */
        config = GPIOx->CRH;
        config &= ~(0xF << ((PinNum - 8) * 4));     /* 清除原有配置 */
        config |= (Mode << ((PinNum - 8) * 4));     /* 设置新模式 */
        GPIOx->CRH = config;
    }

    /* 如果是上拉/下拉输入模式，配置ODR选择上下拉 */
    if (Mode == GPIO_MODE_INPUT_PULL) {
        /* 默认配置为上拉，可通过GPIO_SetPinLevel设置 */
        GPIOx->ODR |= (1 << PinNum);
    }
}

/**
 * @brief 批量初始化GPIO引脚
 * @param configs 配置数组
 * @param count 配置数量
 */
void GPIO_InitMultiple(GPIO_ConfigTypeDef *configs, uint8_t count)
{
    for (uint8_t i = 0; i < count; i++) {
        for (uint8_t pin = 0; pin < 16; pin++) {
            if (configs[i].Pin & (1 << pin)) {
                GPIO_InitPin(configs[i].GPIOx, pin, configs[i].Mode);
            }
        }
    }
}

/**
 * @brief 去初始化GPIO引脚(恢复为浮空输入)
 * @param GPIOx GPIO端口
 * @param PinNum 引脚号
 */
void GPIO_DeInitPin(GPIO_TypeDef *GPIOx, uint8_t PinNum)
{
    GPIO_InitPin(GPIOx, PinNum, GPIO_MODE_INPUT_FLOATING);
}

/* ============================================================================
 * GPIO 输出操作
 * ============================================================================ */

/**
 * @brief 设置引脚输出电平
 * @param GPIOx GPIO端口
 * @param PinNum 引脚号
 * @param Level 电平 (GPIO_LEVEL_LOW 或 GPIO_LEVEL_HIGH)
 */
static inline void GPIO_SetPinLevel(GPIO_TypeDef *GPIOx, uint8_t PinNum, GPIO_LevelTypeDef Level)
{
    if (Level == GPIO_LEVEL_HIGH) {
        GPIOx->BSRR = (1 << PinNum);        /* 设置引脚为高 */
    } else {
        GPIOx->BRR = (1 << PinNum);         /* 设置引脚为低 */
    }
}

/**
 * @brief 设置引脚为高电平
 * @param GPIOx GPIO端口
 * @param PinNum 引脚号
 */
static inline void GPIO_SetPin(GPIO_TypeDef *GPIOx, uint8_t PinNum)
{
    GPIOx->BSRR = (1 << PinNum);
}

/**
 * @brief 清除引脚(设为低电平)
 * @param GPIOx GPIO端口
 * @param PinNum 引脚号
 */
static inline void GPIO_ClearPin(GPIO_TypeDef *GPIOx, uint8_t PinNum)
{
    GPIOx->BRR = (1 << PinNum);
}

/**
 * @brief 翻转引脚电平
 * @param GPIOx GPIO端口
 * @param PinNum 引脚号
 */
static inline void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint8_t PinNum)
{
    GPIOx->ODR ^= (1 << PinNum);
}

/**
 * @brief 写入整个端口的输出数据
 * @param GPIOx GPIO端口
 * @param Value 16位输出值
 */
static inline void GPIO_WritePort(GPIO_TypeDef *GPIOx, uint16_t Value)
{
    GPIOx->ODR = Value;
}

/**
 * @brief 设置端口位(批量)
 * @param GPIOx GPIO端口
 * @param PinMask 引脚位掩码
 */
static inline void GPIO_SetBits(GPIO_TypeDef *GPIOx, uint16_t PinMask)
{
    GPIOx->BSRR = PinMask;
}

/**
 * @brief 清除端口位(批量)
 * @param GPIOx GPIO端口
 * @param PinMask 引脚位掩码
 */
static inline void GPIO_ClearBits(GPIO_TypeDef *GPIOx, uint16_t PinMask)
{
    GPIOx->BRR = PinMask;
}

/* ============================================================================
 * GPIO 输入操作
 * ============================================================================ */

/**
 * @brief 读取单个引脚输入电平
 * @param GPIOx GPIO端口
 * @param PinNum 引脚号
 * @return 0=低电平, 1=高电平
 */
static inline uint8_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint8_t PinNum)
{
    return (GPIOx->IDR & (1 << PinNum)) ? 1 : 0;
}

/**
 * @brief 读取整个端口的输入数据
 * @param GPIOx GPIO端口
 * @return 16位输入值
 */
static inline uint16_t GPIO_ReadPort(GPIO_TypeDef *GPIOx)
{
    return (uint16_t)(GPIOx->IDR);
}

/**
 * @brief 读取单个引脚输出状态
 * @param GPIOx GPIO端口
 * @param PinNum 引脚号
 * @return 0=低, 1=高
 */
static inline uint8_t GPIO_ReadOutputPin(GPIO_TypeDef *GPIOx, uint8_t PinNum)
{
    return (GPIOx->ODR & (1 << PinNum)) ? 1 : 0;
}

/* ============================================================================
 * GPIO 锁定功能
 * ============================================================================ */

/**
 * @brief 锁定GPIO引脚配置
 * @param GPIOx GPIO端口
 * @param PinMask 要锁定的引脚位掩码
 * @return 1=成功, 0=失败
 * @note 锁定后直到下次复位才能修改配置
 */
uint8_t GPIO_LockConfig(GPIO_TypeDef *GPIOx, uint16_t PinMask)
{
    uint32_t tmp = 0x00010000;

    tmp |= PinMask;

    /* 写入LCKR寄存器: 1->0->1 */
    GPIOx->LCKR = tmp;
    GPIOx->LCKR = PinMask;
    GPIOx->LCKR = tmp;

    /* 读取LCKR */
    tmp = GPIOx->LCKR;

    /* 检查锁定是否成功 */
    return (GPIOx->LCKR & 0x00010000) ? 1 : 0;
}

/* ============================================================================
 * 常用GPIO操作封装
 * ============================================================================ */

/**
 * @brief 初始化LED引脚(推挽输出)
 * @param GPIOx GPIO端口
 * @param PinNum 引脚号
 * @param ActiveLow 1=低电平点亮, 0=高电平点亮
 * @details 使用示例:
 *          GPIO_InitLED(GPIOC, 13, 1);  // PC13, 低电平点亮(Blue Pill板载LED)
 */
void GPIO_InitLED(GPIO_TypeDef *GPIOx, uint8_t PinNum, uint8_t ActiveLow)
{
    GPIO_InitPin(GPIOx, PinNum, GPIO_MODE_OUTPUT_PP_2MHz);

    /* 初始状态: 熄灭 */
    if (ActiveLow) {
        GPIO_SetPin(GPIOx, PinNum);     /* 高电平=熄灭 */
    } else {
        GPIO_ClearPin(GPIOx, PinNum);   /* 低电平=熄灭 */
    }
}

/**
 * @brief LED开关控制
 * @param GPIOx GPIO端口
 * @param PinNum 引脚号
 * @param ActiveLow 1=低电平点亮
 * @param On 1=点亮, 0=熄灭
 */
void GPIO_ControlLED(GPIO_TypeDef *GPIOx, uint8_t PinNum, uint8_t ActiveLow, uint8_t On)
{
    if (ActiveLow) {
        if (On) {
            GPIO_ClearPin(GPIOx, PinNum);   /* 低电平点亮 */
        } else {
            GPIO_SetPin(GPIOx, PinNum);     /* 高电平熄灭 */
        }
    } else {
        if (On) {
            GPIO_SetPin(GPIOx, PinNum);     /* 高电平点亮 */
        } else {
            GPIO_ClearPin(GPIOx, PinNum);   /* 低电平熄灭 */
        }
    }
}

/**
 * @brief LED闪烁一次
 * @param GPIOx GPIO端口
 * @param PinNum 引脚号
 * @param ActiveLow 1=低电平点亮
 * @param DelayMs 延时函数指针
 */
void GPIO_BlinkLED(GPIO_TypeDef *GPIOx, uint8_t PinNum, uint8_t ActiveLow,
                   void (*DelayMs)(uint32_t))
{
    GPIO_ControlLED(GPIOx, PinNum, ActiveLow, 1);
    DelayMs(200);
    GPIO_ControlLED(GPIOx, PinNum, ActiveLow, 0);
}

/**
 * @brief 初始化按键引脚
 * @param GPIOx GPIO端口
 * @param PinNum 引脚号
 * @param PullUp 1=上拉输入, 0=下拉输入
 */
void GPIO_InitButton(GPIO_TypeDef *GPIOx, uint8_t PinNum, uint8_t PullUp)
{
    GPIO_InitPin(GPIOx, PinNum, GPIO_MODE_INPUT_PULL);

    /* 设置上拉/下拉 */
    if (PullUp) {
        GPIOx->ODR |= (1 << PinNum);    /* 上拉 */
    } else {
        GPIOx->ODR &= ~(1 << PinNum);   /* 下拉 */
    }
}

/**
 * @brief 读取按键状态(带消抖)
 * @param GPIOx GPIO端口
 * @param PinNum 引脚号
 * @param ActiveLow 1=低电平有效(按下)
 * @param DelayUs 微秒延时函数指针
 * @return 1=按下, 0=未按下
 */
uint8_t GPIO_ReadButton(GPIO_TypeDef *GPIOx, uint8_t PinNum,
                        uint8_t ActiveLow, void (*DelayUs)(uint32_t))
{
    uint8_t level = GPIO_ReadPin(GPIOx, PinNum);

    /* 第一次读取 */
    if ((ActiveLow && level == 0) || (!ActiveLow && level == 1)) {
        DelayUs(1000);  /* 延时1ms消抖 */

        /* 第二次读取确认 */
        level = GPIO_ReadPin(GPIOx, PinNum);
        if ((ActiveLow && level == 0) || (!ActiveLow && level == 1)) {
            return 1;   /* 确认按下 */
        }
    }

    return 0;   /* 未按下 */
}
