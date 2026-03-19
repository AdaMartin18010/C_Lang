/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Multiplatform_Protocols.md
 * Line: 808
 * Language: c
 * Block ID: ad13df8c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// GPIO模式语义详解
// ============================================================

// ========== 输入模式 ==========
// 浮空输入: 外部必须有上拉/下拉，否则悬空易干扰
#define GPIO_MODE_INPUT_FLOATING    0x00

// 上拉输入: 内部上拉电阻(通常30-50kΩ)，默认高电平
#define GPIO_MODE_INPUT_PULLUP      0x01

// 下拉输入: 内部下拉电阻，默认低电平
#define GPIO_MODE_INPUT_PULLDOWN    0x02

// 模拟输入: 连接到ADC，数字功能禁用
#define GPIO_MODE_ANALOG            0x03

// ========== 输出模式 ==========
// 推挽输出: 可主动输出高/低，驱动能力强
//   P-MOS导通 → 输出高(VDD)
//   N-MOS导通 → 输出低(VSS)
#define GPIO_MODE_OUTPUT_PP         0x10

// 开漏输出: 只能拉低或释放(外部上拉决定高电平)
//   应用: I2C、电平转换(如5V→3.3V)、线与逻辑
#define GPIO_MODE_OUTPUT_OD         0x11

// ========== 复用功能 ==========
// 复用推挽: 由外设(USART/SPI等)控制输出
#define GPIO_MODE_AF_PP             0x20

// 复用开漏: I2C必须使用
#define GPIO_MODE_AF_OD             0x21

// ========== 实际应用示例 ==========

// I2C SDA/SCL必须配置为开漏+上拉
void i2c_gpio_config(void) {
    // STM32
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;        // 复用开漏！
    GPIO_InitStruct.Pull = GPIO_PULLUP;            // 必须上拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

// 5V到3.3V电平转换
void level_shifter_config(void) {
    // 开漏输出 + 外部上拉到3.3V
    // MCU(5V容忍)输出开漏 → 外部上拉电阻 → 3.3V器件
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;  // 外部上拉
}

// 按键检测 (上拉输入)
void button_config(void) {
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;  // 按键按下为低电平
}
