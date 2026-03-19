/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\05_MISRA_C_2023_Rules_5.md
 * Line: 489
 * Language: c
 * Block ID: 865fed18
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 正确的柔性数组用法 */
struct packet {
    uint16_t length;
    uint16_t type;
    uint8_t data[];  /* 柔性数组 */
};

/* 分配 */
struct packet *create_packet(uint16_t len)
{
    struct packet *p = malloc(sizeof(struct packet) + len);
    if (p != NULL) {
        p->length = len;
    }
    return p;
}

/* ❌ 错误 - 复制包含柔性数组的结构 */
struct packet *clone = malloc(sizeof(struct packet) + p->length);
*clone = *p;  /* 只复制了前两个字段！ */
memcpy(clone->data, p->data, p->length);  /* 需要手动复制数据 */
