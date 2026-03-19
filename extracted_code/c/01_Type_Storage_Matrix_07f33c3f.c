/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\01_Type_Storage_Matrix.md
 * Line: 453
 * Language: c
 * Block ID: 07f33c3f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// packed_structs.c - 紧凑结构体
#include <stdio.h>
#include <stdint.h>

// 标准结构体 (有填充)
struct StandardPacket {
    uint8_t type;      // 1 byte
    uint32_t length;   // 4 bytes (填充3 bytes)
    uint16_t port;     // 2 bytes (填充2 bytes)
    uint8_t data[6];   // 6 bytes
};
// 总计: 16 bytes (含6 bytes填充)

// 紧凑结构体 - 使用编译器扩展
#ifdef __GNUC__
    #define PACKED __attribute__((packed))
#elif defined(_MSC_VER)
    #define PACKED #pragma pack(push, 1)
    #define PACKED_POP #pragma pack(pop)
#else
    #define PACKED
#endif

struct PACKED PackedPacket {
    uint8_t type;      // 1 byte
    uint32_t length;   // 4 bytes (无填充)
    uint16_t port;     // 2 bytes
    uint8_t data[6];   // 6 bytes
};
// 总计: 13 bytes (无填充)

// 可移植的紧凑结构体 (手动填充)
struct PortablePacket {
    uint8_t type;
    uint8_t length_bytes[4];  // 手动序列化
    uint8_t port_bytes[2];
    uint8_t data[6];
};

// 网络字节序转换函数
uint32_t read_be32(const uint8_t *p) {
    return ((uint32_t)p[0] << 24) |
           ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8) |
           (uint32_t)p[3];
}

void write_be32(uint8_t *p, uint32_t val) {
    p[0] = (val >> 24) & 0xFF;
    p[1] = (val >> 16) & 0xFF;
    p[2] = (val >> 8) & 0xFF;
    p[3] = val & 0xFF;
}

int main(void) {
    printf("=== 结构体大小对比 ===\n");
    printf("sizeof(StandardPacket) = %zu bytes\n", sizeof(struct StandardPacket));
    printf("sizeof(PackedPacket)   = %zu bytes\n", sizeof(struct PackedPacket));
    printf("sizeof(PortablePacket) = %zu bytes\n", sizeof(struct PortablePacket));

    printf("\n=== 紧凑结构体利弊 ===\n");
    printf("优点:\n");
    printf("  - 节省内存\n");
    printf("  - 精确匹配网络/文件协议\n");
    printf("  - 减少内存带宽需求\n");
    printf("\n缺点:\n");
    printf("  - 访问可能变慢(未对齐访问)\n");
    printf("  - 某些平台可能崩溃(如ARM)\n");
    printf("  - 不可移植(编译器扩展)\n");
    printf("  - 调试困难\n");

    return 0;
}
