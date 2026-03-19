/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Multiplatform_Protocols.md
 * Line: 1062
 * Language: c
 * Block ID: fa15254d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 软件I2C (Bit-Bang) 实现
// 用于没有硬件I2C或需要特殊时序的场景
// ============================================================

#include <stdint.h>
#include <stdbool.h>

// I2C引脚定义
#define SDA_PIN     0
#define SCL_PIN     1

// 延时 (根据CPU频率调整)
static inline void i2c_delay(void) {
    for (volatile int i = 0; i < 10; i++);
}

// 设置SDA方向
static inline void sda_output(void) {
    gpio_set_dir(SDA_PIN, GPIO_OUT);
}

static inline void sda_input(void) {
    gpio_set_dir(SDA_PIN, GPIO_IN);
}

// 设置/读取引脚
static inline void sda_high(void) {
    gpio_put(SDA_PIN, 1);
}

static inline void sda_low(void) {
    gpio_put(SDA_PIN, 0);
}

static inline bool sda_read(void) {
    return gpio_get(SDA_PIN);
}

static inline void scl_high(void) {
    gpio_put(SCL_PIN, 1);
}

static inline void scl_low(void) {
    gpio_put(SCL_PIN, 0);
}

// I2C起始条件: SCL高时SDA下降沿
void i2c_start(void) {
    sda_output();
    sda_high();
    scl_high();
    i2c_delay();
    sda_low();
    i2c_delay();
    scl_low();
    i2c_delay();
}

// I2C停止条件: SCL高时SDA上升沿
void i2c_stop(void) {
    sda_output();
    sda_low();
    i2c_delay();
    scl_high();
    i2c_delay();
    sda_high();
    i2c_delay();
}

// 发送一个字节，返回ACK状态
bool i2c_write_byte(uint8_t data) {
    sda_output();

    // 发送8位数据 (MSB first)
    for (int i = 7; i >= 0; i--) {
        if (data & (1 << i)) {
            sda_high();
        } else {
            sda_low();
        }
        i2c_delay();
        scl_high();
        i2c_delay();
        scl_low();
        i2c_delay();
    }

    // 读取ACK
    sda_input();
    sda_high();  // 释放总线
    i2c_delay();
    scl_high();
    i2c_delay();
    bool ack = !sda_read();  // ACK=SDA低
    scl_low();
    i2c_delay();

    return ack;
}

// 读取一个字节
uint8_t i2c_read_byte(bool ack) {
    sda_input();
    sda_high();  // 释放总线

    uint8_t data = 0;

    // 读取8位数据
    for (int i = 7; i >= 0; i--) {
        scl_high();
        i2c_delay();
        if (sda_read()) {
            data |= (1 << i);
        }
        scl_low();
        i2c_delay();
    }

    // 发送ACK/NACK
    sda_output();
    if (ack) {
        sda_low();  // ACK
    } else {
        sda_high(); // NACK
    }
    i2c_delay();
    scl_high();
    i2c_delay();
    scl_low();
    i2c_delay();

    return data;
}

// 完整写入操作
bool i2c_write(uint8_t addr, const uint8_t *data, size_t len) {
    i2c_start();

    // 发送地址+写(0)
    if (!i2c_write_byte((addr << 1) | 0)) {
        i2c_stop();
        return false;  // 设备无响应
    }

    // 发送数据
    for (size_t i = 0; i < len; i++) {
        if (!i2c_write_byte(data[i])) {
            i2c_stop();
            return false;
        }
    }

    i2c_stop();
    return true;
}

// 完整读取操作
bool i2c_read(uint8_t addr, uint8_t *data, size_t len) {
    i2c_start();

    // 发送地址+读(1)
    if (!i2c_write_byte((addr << 1) | 1)) {
        i2c_stop();
        return false;
    }

    // 读取数据
    for (size_t i = 0; i < len; i++) {
        bool ack = (i < len - 1);  // 最后一个字节发NACK
        data[i] = i2c_read_byte(ack);
    }

    i2c_stop();
    return true;
}

// 带寄存器地址的写入 (常用格式)
bool i2c_write_reg(uint8_t addr, uint8_t reg, const uint8_t *data, size_t len) {
    i2c_start();

    if (!i2c_write_byte((addr << 1) | 0)) {
        i2c_stop();
        return false;
    }

    // 发送寄存器地址
    if (!i2c_write_byte(reg)) {
        i2c_stop();
        return false;
    }

    // 发送数据
    for (size_t i = 0; i < len; i++) {
        if (!i2c_write_byte(data[i])) {
            i2c_stop();
            return false;
        }
    }

    i2c_stop();
    return true;
}

// 带寄存器地址的读取
bool i2c_read_reg(uint8_t addr, uint8_t reg, uint8_t *data, size_t len) {
    // 先写寄存器地址
    i2c_start();
    if (!i2c_write_byte((addr << 1) | 0)) {
        i2c_stop();
        return false;
    }
    if (!i2c_write_byte(reg)) {
        i2c_stop();
        return false;
    }

    // 重复起始，然后读
    i2c_start();
    if (!i2c_write_byte((addr << 1) | 1)) {
        i2c_stop();
        return false;
    }

    for (size_t i = 0; i < len; i++) {
        bool ack = (i < len - 1);
        data[i] = i2c_read_byte(ack);
    }

    i2c_stop();
    return true;
}
