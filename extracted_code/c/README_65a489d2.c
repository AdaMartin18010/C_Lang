/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\README.md
 * Line: 247
 * Language: c
 * Block ID: 65a489d2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* safety/safety_mechanisms.h */
#ifndef SAFETY_MECHANISMS_H
#define SAFETY_MECHANISMS_H

#include <stdint.h>
#include <stdbool.h>

/* 安全监控结果 */
typedef enum {
    SAFETY_STATUS_OK,
    SAFETY_STATUS_ERROR_DETECTED,
    SAFETY_STATUS_FAULT
} safety_status_t;

/* 双核锁步 (Dual-Core Lockstep) 监控 */
typedef struct {
    uint32_t core0_result;
    uint32_t core1_result;
    uint32_t compare_count;
    uint32_t mismatch_count;
} lockstep_monitor_t;

safety_status_t lockstep_verify(lockstep_monitor_t* monitor);

/* 端到端保护 (E2E - End-to-End Protection) */
#define E2E_CRC_POLYNOMIAL 0x1021  /* CRC-16-CCITT */

typedef struct {
    uint8_t counter;      /* 序列计数器 */
    uint8_t crc;          /* CRC 校验 */
    uint16_t data_id;     /* 数据标识 */
} e2e_header_t;

uint8_t e2e_calculate_crc(const uint8_t* data, uint32_t length, uint16_t data_id);
bool e2e_verify(const uint8_t* data, uint32_t length, const e2e_header_t* header);
void e2e_protect(uint8_t* data, uint32_t length, uint16_t data_id, e2e_header_t* header);

/* 安全监控器 (Safety Monitor) */
typedef struct {
    uint32_t expected_signature;
    uint32_t actual_signature;
    uint32_t timeout_ms;
    uint32_t last_update_ms;
    bool is_alive;
} safety_monitor_t;

void safety_monitor_init(safety_monitor_t* monitor, uint32_t timeout_ms);
safety_status_t safety_monitor_check(safety_monitor_t* monitor, uint32_t current_time_ms);
void safety_monitor_kick(safety_monitor_t* monitor, uint32_t current_time_ms, uint32_t signature);

#endif
