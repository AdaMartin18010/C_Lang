/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir1.md
 * Line: 167
 * Language: c
 * Block ID: 3c4b0811
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 [C:2023延续] - 未文档化的扩展使用 */
struct __attribute__((packed)) Packet {  /* GCC扩展，未文档化 */
    uint8_t header;
    uint32_t data;
};

/* ❌ 违反 [C:2023延续] - 内联汇编未文档化 */
void delay(void) {
    asm volatile("nop");  /* 未文档化 */
}
