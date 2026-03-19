/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\10_MISRA_C_2023_Rules_10.md
 * Line: 525
 * Language: c
 * Block ID: 6542ae10
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 类型完全一致 */
/* header.h */
#ifndef HEADER_H
#define HEADER_H

#include <stddef.h>
#include <stdint.h>

typedef uint32_t count_t;
extern count_t g_packet_count;

extern int8_t g_temperature;
extern size_t g_buffer_size;

#endif

/* file.c */
#include "header.h"

count_t g_packet_count = 0;
int8_t g_temperature = 0;
size_t g_buffer_size = 1024;
