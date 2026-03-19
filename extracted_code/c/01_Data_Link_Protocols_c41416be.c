/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\Data_Link_Layer\01_Data_Link_Protocols.md
 * Line: 4284
 * Language: c
 * Block ID: c41416be
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file crc_algorithms.c
 * @brief CRC Algorithms Implementation
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * CRC-8 Implementations
 * ============================================================================ */

/**
 * @brief Calculate CRC-8 (ATM/SDH)
 * Polynomial: x^8 + x^2 + x + 1 (0x07)
 * @param data Input data
 * @param len Data length
 * @return CRC-8 value
 */
uint8_t crc8_sdh(const uint8_t *data, size_t len) {
    uint8_t crc = 0x00;

    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x07;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

/**
 * @brief Calculate CRC-8 (CRC-8-CCITT)
 * Polynomial: x^8 + x^7 + x^3 + x^2 + 1 (0x8D)
 * @param data Input data
 * @param len Data length
 * @return CRC-8 value
 */
uint8_t crc8_ccitt(const uint8_t *data, size_t len) {
    uint8_t crc = 0x00;

    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x8D;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

/**
 * @brief Calculate CRC-8 (Dallas/Maxim 1-Wire)
 * Polynomial: x^8 + x^5 + x^4 + 1 (0x31)
 * @param data Input data
 * @param len Data length
 * @return CRC-8 value
 */
uint8_t crc8_dallas(const uint8_t *data, size_t len) {
    uint8_t crc = 0x00;

    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x01) {
                crc = (crc >> 1) ^ 0x8C;
            } else {
                crc >>= 1;
            }
        }
    }

    return crc;
}

/* ============================================================================
 * CRC-16 Implementations
 * ============================================================================ */

/**
 * @brief CRC-16 lookup table (CCITT)
 */
static uint16_t crc16_ccitt_table[256];
static bool crc16_ccitt_table_initialized = false;

/**
 * @brief Initialize CRC-16 CCITT lookup table
 */
void crc16_ccitt_init_table(void) {
    if (crc16_ccitt_table_initialized) return;

    const uint16_t polynomial = 0x1021;  /* x^16 + x^12 + x^5 + 1 */

    for (int i = 0; i < 256; i++) {
        uint16_t crc = (uint16_t)(i << 8);
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ polynomial;
            } else {
                crc <<= 1;
            }
        }
        crc16_ccitt_table[i] = crc;
    }

    crc16_ccitt_table_initialized = true;
}

/**
 * @brief Calculate CRC-16 CCITT (0xFFFF initial)
 * @param data Input data
 * @param len Data length
 * @return CRC-16 value
 */
uint16_t crc16_ccitt(const uint8_t *data, size_t len) {
    crc16_ccitt_init_table();

    uint16_t crc = 0xFFFF;

    for (size_t i = 0; i < len; i++) {
        crc = (crc << 8) ^ crc16_ccitt_table[((crc >> 8) ^ data[i]) & 0xFF];
    }

    return crc;
}

/**
 * @brief Calculate CRC-16 CCITT (0x1D0F initial for Modbus)
 * @param data Input data
 * @param len Data length
 * @return CRC-16 value
 */
uint16_t crc16_ccitt_modbus(const uint8_t *data, size_t len) {
    crc16_ccitt_init_table();

    uint16_t crc = 0x1D0F;

    for (size_t i = 0; i < len; i++) {
        crc = (crc << 8) ^ crc16_ccitt_table[((crc >> 8) ^ data[i]) & 0xFF];
    }

    return crc;
}

/**
 * @brief Calculate CRC-16 (IBM/ARC/SDLC)
 * Polynomial: x^16 + x^15 + x^2 + 1 (0x8005)
 * @param data Input data
 * @param len Data length
 * @return CRC-16 value
 */
uint16_t crc16_ibm(const uint8_t *data, size_t len) {
    uint16_t crc = 0x0000;

    for (size_t i = 0; i < len; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x8005;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

/**
 * @brief Calculate CRC-16 (Modbus)
 * @param data Input data
 * @param len Data length
 * @return CRC-16 value
 */
uint16_t crc16_modbus(const uint8_t *data, size_t len) {
    uint16_t crc = 0xFFFF;

    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }

    return crc;
}

/* ============================================================================
 * CRC-32 Implementations
 * ============================================================================ */

/**
 * @brief CRC-32 lookup table (Ethernet)
 */
static uint32_t crc32_table[256];
static bool crc32_table_initialized = false;

/**
 * @brief Initialize CRC-32 lookup table
 */
void crc32_init_table(void) {
    if (crc32_table_initialized) return;

    const uint32_t polynomial = 0xEDB88320;  /* Reversed polynomial */

    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ polynomial;
            } else {
                crc >>= 1;
            }
        }
        crc32_table[i] = crc;
    }

    crc32_table_initialized = true;
}

/**
 * @brief Calculate CRC-32 (Ethernet/IEEE 802.3)
 * @param data Input data
 * @param len Data length
 * @return CRC-32 value
 */
uint32_t crc32_ethernet(const uint8_t *data, size_t len) {
    crc32_init_table();

    uint32_t crc = 0xFFFFFFFF;

    for (size_t i = 0; i < len; i++) {
        crc = (crc >> 8) ^ crc32_table[(crc ^ data[i]) & 0xFF];
    }

    return crc ^ 0xFFFFFFFF;
}

/**
 * @brief Calculate CRC-32 (POSIX cksum)
 * @param data Input data
 * @param len Data length
 * @return CRC-32 value
 */
uint32_t crc32_posix(const uint8_t *data, size_t len) {
    crc32_init_table();

    uint32_t crc = 0x00000000;

    for (size_t i = 0; i < len; i++) {
        crc = (crc >> 8) ^ crc32_table[(crc ^ data[i]) & 0xFF];
    }

    return crc ^ 0xFFFFFFFF;
}

/**
 * @brief Calculate CRC-32 (MPEG-2)
 * @param data Input data
 * @param len Data length
 * @return CRC-32 value
 */
uint32_t crc32_mpeg2(const uint8_t *data, size_t len) {
    uint32_t crc = 0xFFFFFFFF;
    const uint32_t polynomial = 0x04C11DB7;

    for (size_t i = 0; i < len; i++) {
        crc ^= ((uint32_t)data[i]) << 24;
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80000000) {
                crc = (crc << 1) ^ polynomial;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

/* ============================================================================
 * CRC Test
 * ============================================================================ */

/**
 * @brief CRC test
 */
void crc_test(void) {
    printf("\n=== CRC Algorithms Test ===\n");

    const char *test_data = "123456789";
    size_t len = strlen(test_data);

    printf("Test data: \"%s\" (%zu bytes)\n\n", test_data, len);

    /* CRC-8 tests */
    printf("CRC-8 (SDH/ATM):     0x%02X (expected: 0xF4)\n", crc8_sdh((const uint8_t *)test_data, len));
    printf("CRC-8 (CCITT):       0x%02X\n", crc8_ccitt((const uint8_t *)test_data, len));
    printf("CRC-8 (Dallas):      0x%02X (expected: 0xA1)\n", crc8_dallas((const uint8_t *)test_data, len));

    /* CRC-16 tests */
    printf("\nCRC-16 (CCITT):      0x%04X (expected: 0xE5CC)\n", crc16_ccitt((const uint8_t *)test_data, len));
    printf("CRC-16 (CCITT Modbus):0x%04X\n", crc16_ccitt_modbus((const uint8_t *)test_data, len));
    printf("CRC-16 (IBM):        0x%04X (expected: 0xBB3D)\n", crc16_ibm((const uint8_t *)test_data, len));
    printf("CRC-16 (Modbus):     0x%04X (expected: 0x4B37)\n", crc16_modbus((const uint8_t *)test_data, len));

    /* CRC-32 tests */
    printf("\nCRC-32 (Ethernet):   0x%08X (expected: 0xCBF43926)\n", crc32_ethernet((const uint8_t *)test_data, len));
    printf("CRC-32 (POSIX):      0x%08X\n", crc32_posix((const uint8_t *)test_data, len));
    printf("CRC-32 (MPEG-2):     0x%08X (expected: 0x0376E6E7)\n", crc32_mpeg2((const uint8_t *)test_data, len));

    /* Verify with appended CRC */
    printf("\n--- Verification Test ---\n");
    uint32_t crc = crc32_ethernet((const uint8_t *)test_data, len);
    uint8_t data_with_crc[64];
    memcpy(data_with_crc, test_data, len);
    data_with_crc[len] = crc & 0xFF;
    data_with_crc[len + 1] = (crc >> 8) & 0xFF;
    data_with_crc[len + 2] = (crc >> 16) & 0xFF;
    data_with_crc[len + 3] = (crc >> 24) & 0xFF;

    uint32_t verify = crc32_ethernet(data_with_crc, len + 4);
    printf("Verification CRC: 0x%08X (should be 0x2144DF1C for Ethernet)\n", verify);

    printf("\nCRC test completed\n");
}

/* Main entry point */
int main(void) {
    crc_test();
    return 0;
}
