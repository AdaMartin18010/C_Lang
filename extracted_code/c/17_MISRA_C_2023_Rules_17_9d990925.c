/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\17_MISRA_C_2023_Rules_17.md
 * Line: 357
 * Language: c
 * Block ID: 9d990925
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 柔性数组正确使用 */
struct Packet {
    uint32_t length;
    uint16_t type;
    uint8_t data[];  /* 柔性数组 */
};

/* 分配 */
struct Packet *create_packet(uint32_t data_len)
{
    struct Packet *p = malloc(sizeof(struct Packet) + data_len);
    if (p == NULL) {
        return NULL;
    }

    p->length = data_len;
    p->type = 0;
    /* data自动有data_len字节空间 */

    return p;
}

/* ✅ 合规 - 复制柔性数组结构 */
struct Packet *clone_packet(const struct Packet *src)
{
    if (src == NULL) {
        return NULL;
    }

    struct Packet *dst = malloc(sizeof(struct Packet) + src->length);
    if (dst == NULL) {
        return NULL;
    }

    /* 逐个成员复制，不直接*dst = *src */
    dst->length = src->length;
    dst->type = src->type;
    memcpy(dst->data, src->data, src->length);

    return dst;
}
