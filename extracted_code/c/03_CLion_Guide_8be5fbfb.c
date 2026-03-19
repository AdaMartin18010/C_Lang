/*
 * Auto-generated from: 07_Modern_Toolchain\01_IDE_Editors\03_CLion_Guide.md
 * Line: 309
 * Language: c
 * Block ID: 8be5fbfb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    int id;
    char name[256];
    double value;
} sensor_data_t;

sensor_data_t sensor = {0};

// 在CLion中设置数据断点监控 sensor.value 的变化
// 当任何代码修改 sensor.value 时，调试器会暂停
