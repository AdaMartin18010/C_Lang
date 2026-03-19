/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\04_MISRA_C_2025_Rules_16-22.md
 * Line: 613
 * Language: c
 * Block ID: 91052129
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C23 */
static const uint8_t firmware[] = {
    #embed "firmware.bin"
};

/* C17兼容 - 使用外部工具生成 */
static const uint8_t firmware[] = {
    #include "firmware_data.h"
};
