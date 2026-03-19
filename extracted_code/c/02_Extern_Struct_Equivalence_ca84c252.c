/*
 * Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\02_Extern_Struct_Equivalence.md
 * Line: 1595
 * Language: c
 * Block ID: ca84c252
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码 - 可移植结构体定义策略
#ifndef PORTABLE_STRUCT_H
#define PORTABLE_STRUCT_H

#include <stdint.h>
#include <stdalign.h>

// 策略 1: 使用固定宽度类型
struct PortableHeader {
    uint32_t magic;      // 4 bytes, 固定大小
    uint32_t version;    // 4 bytes
    uint64_t timestamp;  // 8 bytes
    uint64_t data_size;  // 8 bytes
};
// 此结构体在 LP64/LLP64/ILP32 上大小和布局相同

// 策略 2: 显式填充确保布局一致
struct WithPadding {
    uint16_t id;       // 2 bytes
    uint16_t _pad1;    // 显式填充
    uint32_t flags;    // 4 bytes
    uint64_t offset;   // 8 bytes
};
// 总大小: 16 bytes，所有平台一致

// 策略 3: 使用 packed 结构体 (网络/文件格式)
#ifdef _MSC_VER
    #define PACKED __pragma(pack(push, 1))
    #define PACKED_END __pragma(pack(pop))
#else
    #define PACKED __attribute__((packed))
    #define PACKED_END
#endif

struct PACKED NetworkPacket {
    uint8_t type;
    uint16_t length;  // 2 bytes，可能在非对齐地址
    uint32_t seq;
} PACKED_END;

// 策略 4: 编译时断言验证布局
_Static_assert(sizeof(struct PortableHeader) == 24, "Unexpected size");
_Static_assert(alignof(struct PortableHeader) == 8, "Unexpected alignment");

// 策略 5: 序列化/反序列化函数 (最可移植)
struct SerializeBuf {
    uint8_t *data;
    size_t len;
};

static inline void write_u32(uint8_t **p, uint32_t v) {
    (*p)[0] = (v >> 24) & 0xFF;
    (*p)[1] = (v >> 16) & 0xFF;
    (*p)[2] = (v >> 8)  & 0xFF;
    (*p)[3] = v & 0xFF;
    *p += 4;
}

static inline uint32_t read_u32(const uint8_t **p) {
    uint32_t v = ((uint32_t)(*p)[0] << 24) |
                 ((uint32_t)(*p)[1] << 16) |
                 ((uint32_t)(*p)[2] << 8)  |
                 (uint32_t)(*p)[3];
    *p += 4;
    return v;
}

#endif
