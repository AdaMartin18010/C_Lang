/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Depth.md
 * Line: 1904
 * Language: c
 * Block ID: cdad3b7b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ UNSAFE: 未对齐访问（在严格对齐架构上崩溃） */
void unaligned_access(void) {
    char buf[10];
    int *p = (int *)(buf + 1);  /* 地址可能不对齐 */
    *p = 42;                     /* UB: 可能总线错误！ */
}

/* ❌ UNSAFE: 网络协议解析常见错误 */
void parse_packet_bad(const uint8_t *data) {
    uint32_t value = *(uint32_t *)(data + 3);  /* 3字节偏移，不对齐 */
    printf("%u\n", value);
}

/* ❌ UNSAFE: Packed结构体强制转换 */
#pragma pack(push, 1)
struct PackedHeader {
    uint8_t type;
    uint32_t length;  /* 偏移1，不对齐 */
};
#pragma pack(pop)

void packed_access(void) {
    char buf[100];
    struct PackedHeader *h = (struct PackedHeader *)buf;
    h->length = 1000;  /* UB: 写入不对齐地址 */
}

/* ✅ SAFE: 使用memcpy */
#include <string.h>

void parse_packet_safe(const uint8_t *data) {
    uint32_t value;
    memcpy(&value, data + 3, sizeof(value));  /* 总是安全的 */
    /* 如果需要考虑字节序 */
    value = ((uint32_t)data[3] << 24) |
            ((uint32_t)data[4] << 16) |
            ((uint32_t)data[5] << 8)  |
            (uint32_t)data[6];
    printf("%u\n", value);
}

/* ✅ SAFE: 手动字节操作 */
uint32_t read_unaligned_u32(const uint8_t *p) {
    return ((uint32_t)p[0] << 0)  |
           ((uint32_t)p[1] << 8)  |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

/* ✅ SAFE: 使用编译器扩展（GCC/Clang） */
#if defined(__GNUC__)
    #define UNALIGNED_READ32(p) __builtin_memcpy(&(uint32_t){0}, (p), 4)
#else
    #define UNALIGNED_READ32(p) read_unaligned_u32(p)
#endif
