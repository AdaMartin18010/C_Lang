/*
 * Auto-generated from: 01_Core_Knowledge_System\06_Advanced_Layer\03_Portability.md
 * Line: 213
 * Language: c
 * Block ID: f4248544
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 假设小端
uint32_t value = 0x12345678;
uint8_t *p = (uint8_t *)&value;
if (p[0] == 0x78) { /* 小端 */ }

// ✅ 显式字节序处理
uint32_t to_big_endian(uint32_t x) {
    return ((x & 0xFF000000) >> 24) |
           ((x & 0x00FF0000) >> 8)  |
           ((x & 0x0000FF00) << 8)  |
           ((x & 0x000000FF) << 24);
}
