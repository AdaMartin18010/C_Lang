/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\04_MISRA_C_2023_Rules_4.md
 * Line: 277
 * Language: c
 * Block ID: b5d894cf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief Maximum number of sensors supported
 *
 * This constant defines the maximum number of sensors
 * that can be connected to the system simultaneously.
 *
 * @note Increasing this value will increase RAM usage
 * @see sensor_init(), sensor_count
 */
#define MAX_SENSORS 16

/**
 * @brief System state enumeration
 *
 * Defines the possible states of the system state machine.
 */
enum system_state {
    STATE_INIT,      /**< Initializing */
    STATE_IDLE,      /**< Ready for operation */
    STATE_RUNNING,   /**< Normal operation */
    STATE_ERROR,     /**< Error condition */
    STATE_SHUTDOWN   /**< Shutting down */
};

/**
 * @brief Sensor data structure
 *
 * Contains the data read from a physical sensor.
 * All values are in SI units.
 */
struct sensor_data {
    float temperature;    /**< Temperature in Celsius */
    float humidity;       /**< Relative humidity in % */
    float pressure;       /**< Pressure in hPa */
    uint32_t timestamp;   /**< Unix timestamp of reading */
};
