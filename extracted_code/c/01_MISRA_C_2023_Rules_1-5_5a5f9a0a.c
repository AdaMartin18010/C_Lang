/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\01_MISRA_C_2023_Rules_1-5.md
 * Line: 98
 * Language: c
 * Block ID: 5a5f9a0a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 手动填充 */
struct Packet {
    uint8_t header;
    uint8_t _padding[3];
    uint32_t data;
};

_Static_assert(sizeof(struct Packet) == 8, "Size check");
