/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Multiplatform_Protocols.md
 * Line: 1299
 * Language: c
 * Block ID: 435fb7ac
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// BMP280驱动完整实现
// I2C地址: 0x76 (SDO接地) 或 0x77 (SDO接VCC)
// ============================================================

#define BMP280_ADDR         0x76
#define BMP280_REG_ID       0xD0
#define BMP280_REG_RESET    0xE0
#define BMP280_REG_STATUS   0xF3
#define BMP280_REG_CTRL     0xF4
#define BMP280_REG_CONFIG   0xF5
#define BMP280_REG_DATA     0xF7
#define BMP280_REG_CALIB    0x88

#define BMP280_CHIP_ID      0x58

typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
    int32_t t_fine;  // 中间计算值
} BMP280_CalibData;

static BMP280_CalibData calib;

// 读取校准数据
bool bmp280_read_calibration(void) {
    uint8_t calib_raw[24];

    if (!i2c_read_reg(BMP280_ADDR, BMP280_REG_CALIB, calib_raw, 24)) {
        return false;
    }

    // 解析校准数据 (小端)
    calib.dig_T1 = (calib_raw[1] << 8) | calib_raw[0];
    calib.dig_T2 = (calib_raw[3] << 8) | calib_raw[2];
    calib.dig_T3 = (calib_raw[5] << 8) | calib_raw[4];
    calib.dig_P1 = (calib_raw[7] << 8) | calib_raw[6];
    calib.dig_P2 = (calib_raw[9] << 8) | calib_raw[8];
    calib.dig_P3 = (calib_raw[11] << 8) | calib_raw[10];
    calib.dig_P4 = (calib_raw[13] << 8) | calib_raw[12];
    calib.dig_P5 = (calib_raw[15] << 8) | calib_raw[14];
    calib.dig_P6 = (calib_raw[17] << 8) | calib_raw[16];
    calib.dig_P7 = (calib_raw[19] << 8) | calib_raw[18];
    calib.dig_P8 = (calib_raw[21] << 8) | calib_raw[20];
    calib.dig_P9 = (calib_raw[23] << 8) | calib_raw[22];

    return true;
}

// 初始化BMP280
bool bmp280_init(void) {
    // 读取芯片ID
    uint8_t id;
    if (!i2c_read_reg(BMP280_ADDR, BMP280_REG_ID, &id, 1)) {
        return false;
    }
    if (id != BMP280_CHIP_ID) {
        return false;  // 设备不存在
    }

    // 读取校准数据
    if (!bmp280_read_calibration()) {
        return false;
    }

    // 配置: 温度x2过采样, 气压x16过采样, 正常模式
    uint8_t ctrl = (0x02 << 5) | (0x05 << 2) | 0x03;
    if (!i2c_write_reg(BMP280_ADDR, BMP280_REG_CTRL, &ctrl, 1)) {
        return false;
    }

    // 配置IIR滤波器
    uint8_t config = 0x04 << 2;  // IIR系数=16
    if (!i2c_write_reg(BMP280_ADDR, BMP280_REG_CONFIG, &config, 1)) {
        return false;
    }

    return true;
}

// 补偿温度 (BMP280算法)
int32_t bmp280_compensate_T(int32_t adc_T) {
    int32_t var1 = ((((adc_T >> 3) - ((int32_t)calib.dig_T1 << 1))) *
                    ((int32_t)calib.dig_T2)) >> 11;
    int32_t var2 = (((((adc_T >> 4) - ((int32_t)calib.dig_T1)) *
                      ((adc_T >> 4) - ((int32_t)calib.dig_T1))) >> 12) *
                    ((int32_t)calib.dig_T3)) >> 14;
    calib.t_fine = var1 + var2;
    return (calib.t_fine * 5 + 128) >> 8;  // 返回0.01°C
}

// 补偿气压
uint32_t bmp280_compensate_P(int32_t adc_P) {
    int64_t var1, var2, p;

    var1 = ((int64_t)calib.t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)calib.dig_P3) >> 8) +
           ((var1 * (int64_t)calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)calib.dig_P1) >> 33;

    if (var1 == 0) return 0;

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)calib.dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)calib.dig_P7) << 4);
    return (uint32_t)p;  // 返回Pa (帕斯卡)
}

// 读取测量值
bool bmp280_read(int32_t *temperature, uint32_t *pressure) {
    uint8_t data[6];

    if (!i2c_read_reg(BMP280_ADDR, BMP280_REG_DATA, data, 6)) {
        return false;
    }

    int32_t adc_P = ((int32_t)data[0] << 12) | ((int32_t)data[1] << 4) | (data[2] >> 4);
    int32_t adc_T = ((int32_t)data[3] << 12) | ((int32_t)data[4] << 4) | (data[5] >> 4);

    *temperature = bmp280_compensate_T(adc_T);    // 0.01°C
    *pressure = bmp280_compensate_P(adc_P);       // Pa

    return true;
}
