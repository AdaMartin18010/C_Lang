/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\03_MISRA_C_2023_Rules_3.md
 * Line: 334
 * Language: c
 * Block ID: 9db532a2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file sensor_driver.h
 * @brief Temperature and humidity sensor driver
 * @author Your Name
 * @date 2024-01-15
 * @version 1.0
 *
 * This driver provides an interface to the BME280
 * temperature, humidity, and pressure sensor.
 */

#ifndef SENSOR_DRIVER_H
#define SENSOR_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Sensor operation result codes
 */
enum sensor_result {
    SENSOR_OK = 0,           /**< Operation successful */
    SENSOR_ERROR_INIT,       /**< Initialization failed */
    SENSOR_ERROR_READ,       /**< Read operation failed */
    SENSOR_ERROR_TIMEOUT,    /**< Communication timeout */
    SENSOR_ERROR_CRC         /**< CRC check failed */
};

/**
 * @brief Sensor configuration structure
 *
 * Configuration parameters for the sensor.
 * Use sensor_default_config() for default values.
 */
struct sensor_config {
    uint8_t i2c_address;     /**< I2C device address (0x76 or 0x77) */
    uint32_t timeout_ms;     /**< Communication timeout in milliseconds */
    bool oversampling;       /**< Enable oversampling for better accuracy */
};

/**
 * @brief Sensor data structure
 */
struct sensor_data {
    float temperature;       /**< Temperature in Celsius */
    float humidity;          /**< Relative humidity in percentage */
    float pressure;          /**< Pressure in hPa */
    uint32_t timestamp;      /**< Reading timestamp */
};

/**
 * @brief Get default sensor configuration
 *
 * @return Default configuration with:
 *         - i2c_address: 0x76
 *         - timeout_ms: 1000
 *         - oversampling: true
 */
struct sensor_config sensor_default_config(void);

/**
 * @brief Initialize the sensor
 *
 * @param[in] config Pointer to configuration structure
 * @return SENSOR_OK on success, error code otherwise
 *
 * @note Must be called before any other sensor functions
 * @warning This function is not thread-safe
 *
 * Example usage:
 * @code
 * struct sensor_config cfg = sensor_default_config();
 * cfg.timeout_ms = 2000;
 *
 * enum sensor_result rc = sensor_init(&cfg);
 * if (rc != SENSOR_OK) {
 *     printf("Init failed: %d\n", rc);
 *     return -1;
 * }
 * @endcode
 */
enum sensor_result sensor_init(const struct sensor_config *config);

/**
 * @brief Read sensor data
 *
 * @param[out] data Pointer to data structure to fill
 * @return SENSOR_OK on success, error code otherwise
 *
 * @pre sensor_init() must have been called successfully
 * @post data->timestamp is set to current time
 *
 * @code
 * struct sensor_data data;
 * if (sensor_read(&data) == SENSOR_OK) {
 *     printf("Temp: %.2f C\n", data.temperature);
 * }
 * @endcode
 */
enum sensor_result sensor_read(struct sensor_data *data);

/**
 * @brief Deinitialize the sensor
 *
 * Releases resources allocated by sensor_init().
 * Safe to call even if sensor_init() failed.
 */
void sensor_deinit(void);

#endif /* SENSOR_DRIVER_H */
