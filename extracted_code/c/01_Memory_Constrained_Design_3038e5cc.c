/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\01_Memory_Constrained_Design.md
 * Line: 1814
 * Language: c
 * Block ID: 3038e5cc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 条件编译配置 - 按需编译功能
 * ============================================================================ */

/* config.h - 系统配置文件 */
#ifndef CONFIG_H
#define CONFIG_H

/* 版本信息 */
#define FIRMWARE_VERSION_MAJOR 1
#define FIRMWARE_VERSION_MINOR 2
#define FIRMWARE_VERSION_PATCH 3

/* 功能开关 - 未使用的功能不编译 */
#define ENABLE_UART0            1   /* 串口0 */
#define ENABLE_UART1            0   /* 串口1 - 节省空间 */
#define ENABLE_SPI              1   /* SPI接口 */
#define ENABLE_I2C              1   /* I2C接口 */
#define ENABLE_ADC              1   /* ADC转换 */
#define ENABLE_PWM              0   /* PWM输出 - 不需要 */
#define ENABLE_WATCHDOG         1   /* 看门狗 */
#define ENABLE_RTC              0   /* 实时时钟 - 不需要 */

/* 通信协议选择 */
#define ENABLE_MODBUS           1
#define ENABLE_CANBUS           0   /* 不需要 */
#define ENABLE_MQTT             0   /* 太占空间 */

/* 调试功能 */
#define ENABLE_DEBUG_PRINT      1
#define ENABLE_ASSERT           1   /* 调试版本开启 */
#define ENABLE_ERROR_LOG        1
#define ENABLE_PERF_COUNTERS    0   /* 发布版本关闭 */

/* 模块大小限制 */
#define MAX_SENSORS             4   /* 而不是8或16 */
#define MAX_ACTUATORS           2   /* 限制执行器数量 */
#define UART_RX_BUFFER_SIZE     64  /* 而不是256 */
#define UART_TX_BUFFER_SIZE     32  /* 小缓冲区 */
#define MAX_PACKET_SIZE         128 /* 限制包大小 */
#define MAX_CONNECTIONS         2   /* 连接限制 */

/* 时间配置 */
#define MAIN_LOOP_INTERVAL_MS   10
#define SENSOR_SAMPLE_INTERVAL_MS 100
#define HEARTBEAT_INTERVAL_MS   1000

/* 电源管理 */
#define ENABLE_SLEEP_MODE       1
#define SLEEP_TIMEOUT_MS        5000
#define ENABLE_DYNAMIC_CLOCK    0   /* 简化时钟管理 */

#endif /* CONFIG_H */

/* 条件编译使用示例 */
#include "config.h"

/* UART模块 */
#if ENABLE_UART0
    void uart0_init(uint32_t baudrate);
    void uart0_send(uint8_t data);
    int uart0_recv(void);
#else
    #define uart0_init(x)   ((void)0)
    #define uart0_send(x)   ((void)0)
    #define uart0_recv()    (-1)
#endif

#if ENABLE_UART1
    void uart1_init(uint32_t baudrate);
    void uart1_send(uint8_t data);
    int uart1_recv(void);
#else
    #define uart1_init(x)   ((void)0)
    #define uart1_send(x)   ((void)0)
    #define uart1_recv()    (-1)
#endif

/* 调试打印 */
#if ENABLE_DEBUG_PRINT
    #include "mini_printf.h"
    #define DEBUG_PRINT(fmt, ...) mini_printf("[D] " fmt "\r\n", ##__VA_ARGS__)
    #define INFO_PRINT(fmt, ...)  mini_printf("[I] " fmt "\r\n", ##__VA_ARGS__)
    #define WARN_PRINT(fmt, ...)  mini_printf("[W] " fmt "\r\n", ##__VA_ARGS__)
    #define ERROR_PRINT(fmt, ...) mini_printf("[E] " fmt "\r\n", ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...) ((void)0)
    #define INFO_PRINT(fmt, ...)  ((void)0)
    #define WARN_PRINT(fmt, ...)  ((void)0)
    #define ERROR_PRINT(fmt, ...) ((void)0)
#endif

/* 断言 */
#if ENABLE_ASSERT
    #define ASSERT(cond) \
        do { \
            if (!(cond)) { \
                ERROR_PRINT("ASSERT failed: %s:%d: %s", __FILE__, __LINE__, #cond); \
                while(1); /* 死循环便于调试 */ \
            } \
        } while(0)
#else
    #define ASSERT(cond) ((void)0)
#endif

/* 使用示例 */
void system_init(void)
{
    INFO_PRINT("System init starting, version %d.%d.%d",
               FIRMWARE_VERSION_MAJOR,
               FIRMWARE_VERSION_MINOR,
               FIRMWARE_VERSION_PATCH);

    #if ENABLE_UART0
    uart0_init(115200);
    INFO_PRINT("UART0 initialized at 115200 baud");
    #endif

    #if ENABLE_UART1
    uart1_init(9600);
    INFO_PRINT("UART1 initialized at 9600 baud");
    #endif

    #if ENABLE_SPI
    spi_init();
    DEBUG_PRINT("SPI initialized");
    #endif

    #if ENABLE_I2C
    i2c_init(100000);
    DEBUG_PRINT("I2C initialized at 100kHz");
    #endif

    ASSERT(MAX_SENSORS <= 8);  /* 编译时检查 */
}
