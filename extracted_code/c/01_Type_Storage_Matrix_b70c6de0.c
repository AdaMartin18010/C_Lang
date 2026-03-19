/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\01_Type_Storage_Matrix.md
 * Line: 378
 * Language: c
 * Block ID: b70c6de0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// bitfields.c - 位域使用详解
#include <stdio.h>
#include <stdint.h>

// 位域结构体 - 用于硬件寄存器映射
struct RegisterFlags {
    unsigned int enable : 1;      // 1 bit: 使能标志
    unsigned int mode : 3;        // 3 bits: 工作模式 (0-7)
    unsigned int priority : 4;    // 4 bits: 优先级 (0-15)
    unsigned int reserved : 8;    // 8 bits: 保留
    unsigned int status : 16;     // 16 bits: 状态码
};

// 内存紧凑的标志结构
struct CompactFlags {
    uint8_t is_active : 1;
    uint8_t is_visible : 1;
    uint8_t is_enabled : 1;
    uint8_t reserved : 5;
};

// 网络协议头示例 (IPv4头部简化)
struct IPHeader {
#if defined(__LITTLE_ENDIAN__)
    unsigned int ihl : 4;         // 头部长度
    unsigned int version : 4;     // 版本
#elif defined(__BIG_ENDIAN__)
    unsigned int version : 4;
    unsigned int ihl : 4;
#else
    #error "Unknown endianness"
#endif
    unsigned int tos : 8;         // 服务类型
    unsigned int total_length : 16;
    // ... 更多字段
};

// 位域操作示例
void bitfield_demo(void) {
    struct RegisterFlags reg = {0};

    printf("=== 位域操作 ===\n");
    printf("sizeof(struct RegisterFlags) = %zu bytes\n", sizeof(reg));

    // 设置位域
    reg.enable = 1;
    reg.mode = 5;
    reg.priority = 10;

    printf("enable = %u\n", reg.enable);
    printf("mode = %u\n", reg.mode);
    printf("priority = %u\n", reg.priority);

    // 位域溢出行为 (实现定义)
    reg.mode = 10;  // 超过3位最大值(7)
    printf("mode overflow = %u (truncated)\n", reg.mode);
}

int main(void) {
    bitfield_demo();

    printf("\n=== 位域注意事项 ===\n");
    printf("1. 位域布局是实现定义的，不可移植\n");
    printf("2. 不能取位域的地址 (&reg.enable 非法)\n");
    printf("3. 位域成员必须是整型类型\n");
    printf("4. 无名位域可用于填充对齐\n");
    printf("5. 零宽度位域强制对齐到下一个存储单元\n");

    return 0;
}
