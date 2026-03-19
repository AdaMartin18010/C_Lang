/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\17_MISRA_C_2023_Rules_17.md
 * Line: 70
 * Language: c
 * Block ID: 92b55fbb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 策略1: 按大小排序成员（最小化填充）*/
struct Efficient {
    uint8_t  flags;      /* 1 byte */
    uint8_t  reserved;   /* 1 byte - 填充 */
    uint16_t count;      /* 2 bytes */
    uint32_t data;       /* 4 bytes */
    /* 总大小: 8字节，无浪费 */
};

/* 策略2: 显式填充字段 */
struct Documented {
    uint8_t  flags;
    uint8_t  _padding1[3];  /* 显式填充 */
    uint32_t data;
};

/* 策略3: 使用packed（网络/硬件接口）*/
struct WireFormat {
    uint8_t  type;
    uint32_t value;  /* 通常会有3字节填充 */
} __attribute__((packed));

/* 策略4: 序列化函数（推荐）*/
void serialize_header(const struct Header *h, uint8_t *buffer)
{
    buffer[0] = h->type;
    buffer[1] = (h->value >> 24) & 0xFF;
    buffer[2] = (h->value >> 16) & 0xFF;
    buffer[3] = (h->value >> 8) & 0xFF;
    buffer[4] = h->value & 0xFF;
}

void deserialize_header(const uint8_t *buffer, struct Header *h)
{
    h->type = buffer[0];
    h->value = ((uint32_t)buffer[1] << 24) |
               ((uint32_t)buffer[2] << 16) |
               ((uint32_t)buffer[3] << 8) |
               (uint32_t)buffer[4];
}
