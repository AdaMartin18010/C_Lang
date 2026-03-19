/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\01_MISRA_C_2023_Rules_1-5.md
 * Line: 88
 * Language: c
 * Block ID: 29067d0b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - GCC packed属性 */
struct __attribute__((packed)) Packet {
    uint8_t header;
    uint32_t data;
};
