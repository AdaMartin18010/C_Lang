/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Automotive_ECU\04_OBD_Diagnostics.md
 * Line: 257
 * Language: c
 * Block ID: e95e9079
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// VIN (车辆识别号) 读取
// VIN为17位ASCII字符串

// 请求: 09 02
// 响应包含多帧数据

#define VIN_LENGTH 17

struct vin_info {
    char vin[VIN_LENGTH + 1];
    uint16_t year;
    char manufacturer[4];
    char model[10];
};

// 解析VIN
int parse_vin(const uint8_t* data, size_t len, struct vin_info* info) {
    if (len < VIN_LENGTH) return -1;

    // 跳过服务响应字节，复制VIN
    memcpy(info->vin, data, VIN_LENGTH);
    info->vin[VIN_LENGTH] = '\0';

    // 解析年份 (VIN第10位)
    char year_code = info->vin[9];
    static const int year_map[] = {
        2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017,
        2018, 2019, 2020, 2021, 2022, 2023, 2024, 2025
    };

    if (year_code >= 'A' && year_code <= 'P') {
        info->year = year_map[year_code - 'A'];
    } else if (year_code >= '0' && year_code <= '9') {
        info->year = 2000 + (year_code - '0');
    }

    return 0;
}
