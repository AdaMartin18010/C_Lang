/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Automotive_ECU\04_OBD_Diagnostics.md
 * Line: 442
 * Language: c
 * Block ID: 8e0d9e50
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// obd_parser.h
#ifndef OBD_PARSER_H
#define OBD_PARSER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint16_t rpm;
    uint8_t speed;
    int8_t coolant_temp;
    uint8_t throttle_pos;
    uint8_t engine_load;
    uint16_t maf_rate;
    int16_t timing_advance;
} obd_live_data_t;

bool obd_parse_live_data(const uint8_t* raw_data, size_t len,
                         uint8_t pid, obd_live_data_t* data);

#endif
