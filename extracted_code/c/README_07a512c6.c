/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\ISO_26262\README.md
 * Line: 46
 * Language: c
 * Block ID: 07a512c6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ASIL D级别的防御性编程 */
int process_sensor_data(const SensorData *data)
{
    /* NULL检查 */
    if (data == NULL) {
        return ERROR_NULL_POINTER;
    }

    /* 范围检查 */
    if (data->value < SENSOR_MIN || data->value > SENSOR_MAX) {
        return ERROR_OUT_OF_RANGE;
    }

    /* 合理性检查 */
    if (data->timestamp == 0) {
        return ERROR_INVALID_TIMESTAMP;
    }

    return process_valid_data(data);
}
