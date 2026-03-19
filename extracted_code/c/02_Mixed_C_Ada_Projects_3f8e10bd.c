/*
 * Auto-generated from: 17_Ada_SPARK\06_Migration_from_C\02_Mixed_C_Ada_Projects.md
 * Line: 269
 * Language: c
 * Block ID: 3f8e10bd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// sensor_driver.h
#ifndef SENSOR_DRIVER_H
#define SENSOR_DRIVER_H

#include <stdint.h>

typedef struct {
    uint32_t timestamp;
    float temperature;
    float pressure;
    uint8_t status;
} Sensor_Data;

int sensor_init(const char* device_path);
int sensor_read(Sensor_Data* data);
void sensor_close(void);

#endif
