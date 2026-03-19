/*
 * Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\02_Extern_Struct_Equivalence.md
 * Line: 724
 * Language: c
 * Block ID: 88b0d849
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码 - packed 结构体
#include <stdio.h>
#include <stdint.h>

// GCC/Clang 扩展
struct __attribute__((packed)) PackedGCC {
    char c;
    int i;    // 可能位于非对齐地址
    short s;
};
// sizeof = 7 (1 + 4 + 2)，无填充

// MSVC 扩展
#pragma pack(push, 1)
struct PackedMSVC {
    char c;
    int i;
    short s;
};
#pragma pack(pop)

// 标准 C11 _Static_assert 验证大小
_Static_assert(sizeof(struct PackedGCC) == 7, "Packed size mismatch");

// packed 结构体的陷阱
struct __attribute__((packed)) NetworkHeader {
    uint16_t src_port;   // 可能非对齐
    uint16_t dst_port;
    uint32_t seq_num;    // offset 4，可能非对齐
};

// 非对齐访问可能导致性能问题或崩溃
void process_header(volatile struct NetworkHeader *hdr) {
    // 在 ARM 上可能产生对齐异常
    uint32_t seq = hdr->seq_num;  // 潜在的非对齐访问

    // 更安全的做法: 使用 memcpy
    uint32_t safe_seq;
    memcpy(&safe_seq, &hdr->seq_num, sizeof(safe_seq));
}
