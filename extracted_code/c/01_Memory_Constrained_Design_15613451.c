/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\01_Memory_Constrained_Design.md
 * Line: 120
 * Language: c
 * Block ID: 15613451
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 内存受限系统编程黄金法则
 * ============================================================================ */

/* 原则1: 避免动态内存分配 */
/* 错误做法 */
void process_sensor_bad(void) {
    uint8_t *buffer = malloc(64);  /* 运行时分配 - 危险！ */
    if (buffer) {
        read_sensor(buffer, 64);
        free(buffer);  /* 容易遗漏，导致内存泄漏 */
    }
}

/* 正确做法 */
static uint8_t sensor_buffer[64];  /* 静态分配 */
void process_sensor_good(void) {
    read_sensor(sensor_buffer, 64);
    /* 自动管理，无需释放 */
}

/* 原则2: 使用最小足够的数据类型 */
#include <stdint.h>

/* 错误做法 - 浪费内存 */
int global_counter;           /* 可能是16或32位 */
long timestamp;               /* 32位可能足够 */

/* 正确做法 - 精确指定 */
uint8_t  small_counter;       /* 0-255范围 */
uint16_t medium_counter;      /* 0-65535范围 */
uint32_t large_counter;       /* 32位范围 */
int32_t  timestamp_sec;       /* Unix时间戳 */

/* 原则3: 常量数据放入Flash */
/* 错误做法 - 占用RAM */
const char error_msg[] = "Sensor Error\n";  /* 仍在RAM中！ */

/* 正确做法 - 留在Flash (GCC) */
const char error_msg_flash[] __attribute__((section(".rodata"))) = "Sensor Error\n";

/* AVR特定做法 */
#include <avr/pgmspace.h>
const char error_msg_avr[] PROGMEM = "Sensor Error\n";

/* 原则4: 栈上分配临时数据 */
void process_data(void) {
    /* 临时缓冲区放在栈上，函数返回自动释放 */
    uint8_t temp_buffer[32];

    read_data(temp_buffer, 32);
    process(temp_buffer, 32);
    /* temp_buffer自动释放，零开销 */
}

/* 原则5: 复用内存缓冲区 */
static union {
    uint8_t uart_buffer[32];
    uint8_t sensor_buffer[32];
    uint8_t packet_buffer[32];
} shared_buffers;
/* 同一时间只使用一个缓冲区，节省内存 */
