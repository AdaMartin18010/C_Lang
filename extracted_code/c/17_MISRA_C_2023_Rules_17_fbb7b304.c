/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\17_MISRA_C_2023_Rules_17.md
 * Line: 32
 * Language: c
 * Block ID: fbb7b304
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief Packet header structure
 *
 * Memory layout (packed, no padding):
 *   Offset 0:  uint16_t magic    (2 bytes)
 *   Offset 2:  uint16_t version  (2 bytes)
 *   Offset 4:  uint32_t length   (4 bytes)
 *   Offset 8:  uint32_t seq_num  (4 bytes)
 *   Total: 12 bytes
 *
 * @note This structure is packed using __attribute__((packed))
 *       to match wire format exactly.
 */
struct PacketHeader {
    uint16_t magic;
    uint16_t version;
    uint32_t length;
    uint32_t seq_num;
} __attribute__((packed));

/* 静态断言验证大小 */
_Static_assert(sizeof(struct PacketHeader) == 12,
               "PacketHeader must be exactly 12 bytes");
